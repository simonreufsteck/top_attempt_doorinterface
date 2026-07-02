# DoorInterface — Projekt-Handoff

ESP32-Firmware (PlatformIO + Arduino-Framework) zur Steuerung von Türöffnern
(Relais + NUKI Smart Locks über BLE), mit W-LAN-Einrichtung per Captive Portal,
gesicherter Weboberfläche und API/Websocket für lokales Backend.

## Build & Flash

```bash
pio run                 # build
pio run -t upload       # flash
pio device monitor      # serial @ 115200
```

PlatformIO-Konfiguration: `platformio.ini`, env `esp32dev`, `monitor_speed = 115200`.

> Hinweis zur lokalen Installation (vorgefallen): Das ESP32-Framework-Paket
> `framework-arduinoespressif32` war unvollständig installiert (fehlender
> `variants/`-Ordner) und das Python-Modul `intelhex` fehlte für `esptool`.
> Beides wurde manuell behoben (`intelhex` per `pip install` ins PIO-Python-Env).
> Falls der Build auf einem anderen Rechner fehlschlägt, zuerst prüfen:
> `Test-Path "$env:USERPROFILE\.platformio\packages\framework-arduinoespressif32\variants"`

## Code-Struktur (Stand jetzt)

```
platformio.ini
src/
  main.cpp              -> instanziiert WifiManager, ruft begin()/loop()
  WifiManager.h/.cpp    -> WLAN-STA-Versuch + AP-Fallback + Captive Portal
  web/
    portal_html.h       -> Captive-Portal-HTML (PROGMEM)
    portal_css.h        -> Captive-Portal-CSS (PROGMEM)
    portal_js.h         -> Captive-Portal-JS  (PROGMEM)
```

## Architektur-Entscheidungen

- **Webinhalte als PROGMEM-Strings**, nicht als Filesystem (LittleFS).
  Grund: späteres OTA-Update (per GitHub-Push generiertes Binary) soll die
  Webinhalte automatisch mitliefern — eine Daten-Partition via OTA separat
  zu updaten wäre fehleranfälliger. Dateien unter `src/web/`, header-only,
  je ein PROGMEM-String pro Datei.
- **Trennung nach Bereich und Typ**: `portal_*.h` für Captive Portal,
  später `main_*.h` für die Haupt-Weboberfläche. Pro Bereich je eine
  Datei für html/css/js.
- **WifiManager als eigenes Modul** (`src/WifiManager.cpp`), `main.cpp`
  bleibt schlank.
- **Debug-Logging** über `Serial.print*` mit `[WifiManager]`- bzw.
  `[HTTP]`-Präfix (kein zentrales Debug-Makro bisher).

## Captive Portal — implementierter Ablauf

1. `begin()`: NVS-Credentials (Namespace `"wifi"`) laden.
   - NVS wird schreibbar geöffnet (`Preferences::begin("wifi", false)`), damit
     der Namespace beim ersten Start angelegt wird — sonst `nvs_open: NOT_FOUND`.
2. Falls SSID vorhanden: STA-Versuch (`tryConnect`), 15 s Timeout.
   - Erfolg: fertig, ESP läuft im STA-Modus.
   - Misserfolg: `startFallbackAp()`.
3. `startFallbackAp()`: Modus `WIFI_AP_STA` (AP bleibt an, STA kann scannen/
   connecten). AP-Name `DoorSetup-AP` (offen, kein Passwort). Startet Portal.
4. `startPortal()`:
   - `DNSServer` Catch-All (`*` -> AP-IP) = Captive-Portal-Trigger.
   - Routen: `/`, `/portal.css`, `/portal.js`, `/scan`, `/save`, `/status`.
   - Bekannte Captive-Detection-Pfade (`/generate_204`, `/gen_204`,
     `/hotspot-detect.html`, `/ncsi.txt`, `/connecttest.txt`, `/redirect`)
     werden als eigene Routen registriert, um das Library-Log
     "request handler not found" zu vermeiden.
   - `onNotFound` leitet alles übrige mit 302 auf `/` weiter.
5. `/scan` (GET): `WiFi.scanNetworks()` blockierend, Antwort als JSON-Array
   `[{ssid, rssi, secure}]`. Webserver blockiert während Scan (~2-4 s),
   Client-Seite zeigt halbtransparenten Loading-Overlay.
6. `/save` (POST): SSID/Pass in NVS speichern, `WiFi.begin()`, antwortet sofort
   `{"status":"connecting"}` (nicht blockierend).
7. Portal-State-Machine in `loop()`:
   - `PORTAL_CONNECTING`: pollt STA-Status, Timeout 15 s.
   - `PORTAL_CONNECTED`: ab hier 30 s Timer, danach AP abschalten
     (`softAPdisconnect`, `WiFi.mode(WIFI_STA)`), `_apActive=false`.
   - `PORTAL_FAILED`: Formular kann erneut ausgefüllt werden.
8. `/status` (GET): JSON `{state, ip}` für Client-Polling (1 s).
   Bei `connected` zeigt das Frontend die STA-IP als Link an.

## Frontend (Captive Portal)

- HTML/CSS/JS inline als PROGMEM in `src/web/portal_*.h`.
- `-Select` für gescannte Netzwerke (RSSI + Schloss-Icon bei secure),
  zusätzlich manuelles SSID-Feld zum Überschreiben.
- Overlay (`#overlay`) mit Spinner, shown/hidden per `showOverlay/hideOverlay`.
- Status-Polling mit Link auf die zukünftige STA-IP.

## Bisherige Häppchen-Schritte

1. **Schritt 1**: Grundgerüst `WifiManager` (STA-Versuch + AP-Fallback).
2. **Schritt 1b**: Debug-`Serial`-Ausgaben ergänzt.
3. **Schritt 2a**: `src/web/portal_*.h` angelegt (HTML/CSS/JS PROGMEM).
4. **Schritt 2b**: DNS + Webserver + Routen in `WifiManager`, Portal ausliefern.
5. **Schritt 3**: `/save` implementiert (Credentials speichern + connecten).
6. **Schritt 3b**: Scan (`/scan`), Status (`/status`), State-Machine,
   non-blocking connect, Overlay, AP-Auto-Off nach 30 s, IP-Anzeige im Frontend.
7. **Fix**: NVS schreibbar öffnen, Captive-Detection-Pfade registriert.

## Arbeitsweise

- Kleine Häppchen, jeder Schritt als Code-Snippet vorgeschlagen + erklärt,
  erst auf "okay" in Dateien geschrieben.
- Schritte werden vor dem Anlegen erklärt, nicht automatisch committed.
- `git commit` nur auf ausdrücklichen Wunsch.

## Offene TODOs (Reihenfolge grob nach Priorität)

### WLAN / Setup
- [ ] AP-Passwort für Setup-AP konfigurierbar (aktuell offen).
- [ ] Reset-Möglichkeit der gespeicherten WLAN-Credentials (Taster/Erase-Flag),
        damit der ESP bei laufendem STA wieder ins Setup-Portal kommt.
- [ ] Reconnect-Logik bei STA-Verbindungsabbruch (auto-reconnect, retries).
- [ ] mDNS (`MDNS.begin("doorinterface")`) für `http://doorinterface.local`.

### Weboberfläche (Hauptseite, im STA-Modus)
- [ ] `src/web/main_*.h` anlegen (HTML/CSS/JS) für Hauptseite.
- [ ] Eigenen Webserver (oder zweiten) im STA-Modus starten.
- [ ] Login / Session-Auth (z. B. Basic-Auth, Token, Session-Cookie).
- [ ] Seiten: Konfiguration (Türöffner, NUKI-Pairing, Benutzer),
        Steuerung (Tür öffnen), Status (Verbindung, Akku, etc.).
- [ ] API-Endpoints (REST-JSON) mit Auth für Backend-Anbindung.
- [ ] Websocket-Endpoint für Live-Status/Steuerung.

### Türöffner
- [ ] Relais-Ansteuerung (GPIO, Timing, Entstörung).
- [ ] NUKI Smart Lock Integration über BLE (Library-Evaluation: offizielle
        NUKI BLE-API, Community-Libs: `esp-nuki-coder`, `nuki-esp32`, ...).
- [ ] Mehrere Locks parallel: Konfig-Modell (Liste von Lock-Deskriptoren),
        Pairing-Flow, Status-Polling.

### Backend-Anbindung
- [ ] Gessicherte Verbindung vom lokalen Backend zum ESP (TLS? Mutual Auth?).
- [ ] Authentifizierung des Backends gegenüber dem ESP (API-Token, Zertifikat).

### OTA / GitHub-Workflow
- [ ] OTA per Webserver: Binary über POST oder Pull vom GitHub-Release laden.
- [ ] GitHub-Action: bei Push auf `main` Firmware bauen, als Release-Artefakt
        bereitstellen (`.bin`).
- [ ] ESP prüft periodisch GitHub auf neue Version, zeigt an, bieten Update an
        (mit Rollback-Partition).

### Logging / Robustheit
- [ ] Zentrales Debug-Makro (`#define DEBUG_SERIAL` + `LOGI/LOGW/LOGE`),
        um Serial-Logs zentral ein/ausschalten zu können.
- [ ] Awareness: `WebServer _server{80}` als globales Member — bei Wachstum
        ggf. auf `begin()`-Muster mit `new` umstellen (Init-Reihenfolge).

## Konventionen / Notizen

- Keine Kommentare im Code (per Absprache).
- `Serial`-Präfixe: `[WifiManager]`, `[HTTP]`, später z. B. `[Relay]`, `[NUKI]`.
- NVS-Namespaces klein & eindeutig (`"wifi"`, später z. B. `"locks"`, `"auth"`).
- Dateien im `src/`-Verzeichnis, nicht in `lib/` (per Absprache).

## Forsetzung

Nächster empfohlener Schritt nach der Pause:
**gesicherte Weboberfläche im STA-Modus** —>
1. mDNS aktivieren (`MDNS.begin`).
2. Haupt-Webserver (Port 80) starten, wenn STA verbunden.
3. `src/web/main_html.h` etc. anlegen, erste Index-Seite ausliefern.
4. Login/Session als eigener Häppchen-Schritt.