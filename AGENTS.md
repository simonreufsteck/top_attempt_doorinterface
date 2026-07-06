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

> Hinweis: `pio` ist nicht im PATH. PlatformIO liegt unter
> `C:\Users\Simon\.platformio\penv\Scripts\platformio.exe`.

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
  config.h              -> FW_VERSION "0.1.0"
  main.cpp              -> instanziiert WifiManager, NukiManager, WebInterface
  WifiManager.h/.cpp    -> WLAN-STA-Versuch + AP-Fallback + Captive Portal + Hostname
  WebInterface.h/.cpp   -> Haupt-Webserver (STA-Modus): Dashboard, Setup, API
  NukiManager.h/.cpp    -> NUKI BLE: Pairing, Lock/Unlock, Status-Querying
  web/
    portal_html.h       -> Captive-Portal-HTML (PROGMEM)
    portal_css.h        -> Captive-Portal-CSS (PROGMEM)
    portal_js.h         -> Captive-Portal-JS  (PROGMEM)
    main_html.h         -> Dashboard-HTML (PROGMEM)
    main_css.h          -> Dashboard + Setup CSS (PROGMEM)
    main_js.h           -> Dashboard-JS: Status-Poll, Lock/Unlock-Buttons
    setup_html.h        -> Setup-Seite-HTML (PROGMEM)
    setup_js.h          -> Setup-JS: Hostname, NUKI-Pairing, Test-Buttons
lib/
  nuki_ble/             -> Gepatchter Fork von AzonInc/NukiBleEsp32 (idf-Branch)
                           7 NimBLE-API-Patches für NimBLE-Arduino 1.4.x
                           Eigene Preferences.h/.cpp entfernt (Arduino-Framework genutzt)
```

## Architektur-Entscheidungen

- **Webinhalte als PROGMEM-Strings**, nicht als Filesystem (LittleFS).
  Grund: späteres OTA-Update (per GitHub-Push generiertes Binary) soll die
  Webinhalte automatisch mitliefern — eine Daten-Partition via OTA separat
  zu updaten wäre fehleranfälliger. Dateien unter `src/web/`, header-only,
  je ein PROGMEM-String pro Datei.
- **Trennung nach Bereich und Typ**: `portal_*.h` für Captive Portal,
  `main_*.h`/`setup_*.h` für die Haupt-Weboberfläche. Pro Bereich je eine
  Datei für html/css/js.
- **WifiManager als eigenes Modul** (`src/WifiManager.cpp`), `main.cpp`
  bleibt schlank.
- **WebInterface als eigenes Modul** (`src/WebInterface.cpp`), startet wenn
  STA verbunden + AP zu. Nimmt `WifiManager&` und `NukiManager&` per Referenz.
- **NukiManager als eigenes Modul** (`src/NukiManager.cpp`), kapselt BLE-Scanner
  + NukiLock. Event-Handler für Status-Updates. Credentials in NVS (Namespace
  = Gerätename, verwaltet von NukiBleEsp32-Lib).
- **Debug-Logging** über `Serial.print*` mit Präfixen: `[WifiManager]`,
  `[HTTP]`, `[NUKI]`.

## NUKI BLE Integration — wichtige Details

- **Bibliothek**: `lib/nuki_ble/` = gepatchter Fork von
  `https://github.com/AzonInc/NukiBleEsp32.git` (idf-Branch).
  Unterstützt **alle NUKI-Modelle**: Smart Lock 1.0–4.0, 5.0 Pro, Ultra, Go,
  Opener, Keypad.
- **7 NimBLE-API-Patches** für Kompatibilität mit `NimBLE-Arduino @ ^1.4.1`:
  1. `onDisconnect(BLEClient*, int reason)` → `onDisconnect(BLEClient*)`
  2. `onResult(const BLEAdvertisedDevice*)` → `onResult(BLEAdvertisedDevice*)`
  3. `NimBLERemoteCharacteristic::notify_callback` → `notify_callback`
  4. `NimBLEDevice::isInitialized()` Aufrufe entfernt (nicht in 1.4.x)
  5. `NimBLEDevice::setPower(int)` → `setPower(esp_power_level_t)`
  6. `NimBLEBeacon::setData(uint8_t*, uint8_t)` → `setData(std::string)`
  7. `BLEAddress::getVal()` → `BLEAddress::getNative()`
- **Preferences-Konflikt gelöst**: Die idf-Branch hat eine eigene
  `Preferences.h/.cpp` mit `std::string`-API, die mit der Arduino-Framework-
  `Preferences` (mit `String`-API) kollidiert. Lösung: eigene Dateien
  **gelöscht**, die Arduino-Framework-Version ist ein Drop-in Replacement.
- **Kein Framework-Wechsel**: `framework = arduino` (kein espidf), keine
  sdkconfig.defaults, keine partitions.csv nötig.
- **Dependencies**: `NimBLE-Arduino @ ^1.4.1`, `BleScanner` (I-Connect),
  `Crc16` (vinmenn).
- **Ultra/5th-Gen PIN**: Für Smart Lock Ultra/5th Gen/Go/Pro muss vor dem
  Pairing die 6-stellige PIN gesetzt werden (`saveUltraPincode()`).
  Standard-Locks (1.0–4.0) brauchen keine PIN.
  TODO: PIN-Eingabe in der Setup-Seite.
- **Ultra-Support Forschung**: Platform-Upgrade auf Arduino Core 3.x
  (ESP-IDF 5.x) mit `framework = arduino, espidf` wurde getestet, scheiterte
  aber an Python-Dependency-Problemen in PlatformIO. Die gepatchte Fork-Lösung
  umgeht das vollständig.

## Captive Portal — implementierter Ablauf

1. `begin()`: NVS-Hostname (Namespace `"system"`) + Credentials (Namespace
   `"wifi"`) laden. Hostname-Default: `doorinterface-XXXX` (letzte 2 MAC-Bytes).
2. Falls SSID vorhanden: STA-Versuch (`tryConnect`), 15 s Timeout.
   `WiFi.setHostname()` wird vor jedem `WiFi.begin()` aufgerufen.
3. `startFallbackAp()`: `setAutoReconnect(false)` + `disconnect(false)` →
   STA-Radio frei für Scan. Modus `WIFI_AP_STA`, AP-Name `DoorSetup-AP`.
4. `startPortal()`:
   - `DNSServer` Catch-All (`*` -> AP-IP).
   - Routen: `/`, `/portal.css`, `/portal.js`, `/scan`, `/save`, `/status`,
     `/config`, `/close`.
   - `UriGlob("*")` + `HTTP_ANY` als Catch-All → kein `log_e`-Spam mehr.
   - `onNotFound` als defensive fallback.
5. `/scan` (GET): `WiFi.scanNetworks()` blockierend, JSON-Array.
6. `/save` (POST): SSID/Pass + optional Hostname in NVS, `WiFi.begin()`,
   antwortet sofort `{"status":"connecting"}`.
7. `/config` (GET): JSON `{hostname}` für Portal-Frontend.
8. `/close` (POST): AP sofort schließen (Button im Overlay nach erfolgtem
   Connect). Frontend kopiert STA-IP ins Clipboard + schließt AP.
9. Portal-State-Machine in `loop()`:
   - `_shutdownRequested` Flag → `shutdownAp()` (vom Button und 30s-Timer).
   - `PORTAL_CONNECTING` → `PORTAL_CONNECTED` → 30s Timer → `shutdownAp()`.
   - `shutdownAp()`: `setAutoReconnect(true)` für reinen STA-Betrieb.

## Frontend (Captive Portal)

- HTML/CSS/JS inline als PROGMEM in `src/web/portal_*.h`.
- Select für gescannte Netzwerke + manuelles SSID-Feld.
- Gerätename-Feld (geladen via `/config`).
- Overlay mit Spinner + Status-Polling.
- Bei `connected`: Adresse + „Adresse kopieren & Setup beenden"-Button.

## Haupt-Weboberfläche (STA-Modus)

- **WebInterface** startet wenn `wifi.isConnected() && !wifi.isApActive()`.
- mDNS mit dynamischem Hostnamen (`MDNS.begin(hostname)`).
- Routen: `/`, `/main.css`, `/main.js`, `/setup`, `/setup.js`,
  `/api/status`, `/api/hostname` (GET+POST), `/api/nuki/pair`,
  `/api/nuki/cancel`, `/api/nuki/unlock`, `/api/nuki/lock`.
- `/api/status` JSON: `{wifi, relay, locks, firmware}`.
  `locks`: `{available, count, paired, pairing, lockState, batteryPct,
  batteryCritical, rssi}`.
- Dashboard (`main_js.h`): 3 Karten (WLAN, Türöffner, Firmware).
  - WLAN: Badge + SSID/RSSI/IP, Poll alle 3s.
  - Türöffner: „nicht eingerichtet" + Setup-Link, oder Lock-State-Badge +
    Akku/RSSI + „Öffnen"/„Sperren"-Buttons.
  - ⚙-Dropdown oben rechts → `/setup`.
- Setup-Seite (`setup_js.h`):
  - Gerätename ändern (→ Reboot).
  - NUKI Pairing: Anleitung + „Pairing starten"/„Pairing abbrechen".
  - Test-Buttons (Öffnen/Sperren) bei gepaartem Lock.
  - Poll alle 2s.

## NUKI Pairing-Flow

1. Nuki-App: Bluetooth Pairing aktivieren (Settings → Features & Configuration
   → Button and LED).
2. Nuki-Taste 10s drücken (LED-Ring leuchtet).
3. Setup-Seite → „Pairing starten" → `POST /api/nuki/pair`.
4. `NukiManager::startPairing()` → `_pairingRequested = true`.
5. `loop()` ruft `pairNuki()` auf bis Success oder 10-Min-Timeout.
6. Bei Success: `requestKeyTurnerState()` → Status im Dashboard.
7. „Pairing abbrechen" → `POST /api/nuki/cancel` → `_pairingRequested = false`.
8. Credentials (ECDH-Key, Auth-ID) in NVS gespeichert (von Lib verwaltet).
   Bei Neustart: kein Re-Pairing nötig.

## Bisherige Häppchen-Schritte

1. Grundgerüst `WifiManager` (STA + AP-Fallback).
2. Captive Portal (HTML/CSS/JS, DNS, Routen, Scan, Save, Status, State-Machine).
3. NVS-Fix + Captive-Detection-Pfade.
4. Scan-Fix (`setAutoReconnect(false)` + `disconnect`).
5. `UriGlob("*")`-Catch-All gegen Log-Spam.
6. Portal-Finish-Button (Adresse kopieren + AP schließen).
7. `config.h` mit `FW_VERSION`.
8. `WebInterface`-Modul (Dashboard + mDNS).
9. Hostname-Feature: NVS `"system"`, unique Default, editierbar im Portal +
   Setup-Seite, `WiFi.setHostname()` vor `WiFi.begin()`.
10. NUKI BLE Integration: gepatchter Fork (idf-Branch), `NukiManager`,
    Pairing, Lock/Unlock, Status, Dashboard-Buttons, Setup-Seite.
11. Pairing-Abbruch (`/api/nuki/cancel`).

## Arbeitsweise

- Kleine Häppchen, jeder Schritt als Code-Snippet vorgeschlagen + erklärt,
  erst auf "okay" in Dateien geschrieben.
- Schritte werden vor dem Anlegen erklärt, nicht automatisch committed.
- `git commit` nur auf ausdrücklichen Wunsch.

## Offene TODOs (Reihenfolge grob nach Priorität)

### NUKI
- [ ] PIN-Eingabe für Ultra/5th Gen/Go/Pro in der Setup-Seite
        (`saveUltraPincode()` vor Pairing).
- [ ] Unpair-Funktion (`unPairNuki()` + Setup-Button).
- [ ] Mehrere Locks parallel (Liste von NukiLock-Instanzen am selben Scanner).
- [ ] Keypad-Verwaltung, Auth-Entries, Time-Control.
- [ ] Event-Log (benötigt PIN).

### WLAN / Setup
- [ ] AP-Passwort für Setup-AP konfigurierbar (aktuell offen).
- [ ] Reset-Möglichkeit der gespeicherten WLAN-Credentials (Taster/Erase-Flag).
- [ ] Reconnect-Logik bei STA-Verbindungsabbruch.

### Weboberfläche
- [ ] Login / Session-Auth (z. B. Basic-Auth, Token, Session-Cookie).
- [ ] Relais-Konfiguration (Pin, Pegel) in Setup-Seite.
- [ ] SSID-Escaping im Status-JSON.

### Backend-Anbindung
- [ ] Entscheidung: ESP als WS-Client (Empfehlung bei mehreren ESPs) oder
        WS-Server auf ESP. Serverpod-Backend noch nicht begonnen.
- [ ] Gesicherte Verbindung ESP↔Backend (TLS? Mutual Auth?).
- [ ] Authentifizierung des Backends gegenüber dem ESP (API-Token).

### Relais
- [ ] GPIO-Ansteuerung (Pin, Timing, Entstörung). Braucht Hardware-Info.

### OTA / GitHub-Workflow
- [ ] OTA per Webserver: Binary über POST oder Pull vom GitHub-Release.
- [ ] GitHub-Action: bei Push auf `main` Firmware bauen, als Release-Artefakt.
- [ ] ESP prüft periodisch GitHub auf neue Version.

### Logging / Robustheit
- [ ] Zentrales Debug-Makro (`#define DEBUG_SERIAL` + `LOGI/LOGW/LOGE`).
- [ ] Flash bei 92.2% — knapp. Custom-Partition-Table (kein OTA-Slot →
        2 MB App) oder `-Os` bei Bedarf.

## Konventionen / Notizen

- Keine Kommentare im Code (per Absprache).
- `Serial`-Präfixe: `[WifiManager]`, `[HTTP]`, `[NUKI]`.
- NVS-Namespaces: `"wifi"`, `"system"`, NUKI verwaltet eigene
  (Namespace = Gerätename).
- Dateien im `src/`-Verzeichnis, nicht in `lib/` (per Absprache).
  Ausnahme: `lib/nuki_ble/` = gepatchter Fork (nicht eigenem Code).

## Forsetzung

Nächster empfohlener Schritt:
**NUKI PIN-Eingabe für Ultra/5th Gen** oder **Relais-GPIO** oder
**Zugriffsschutz (Auth)** — je nach Priorität.