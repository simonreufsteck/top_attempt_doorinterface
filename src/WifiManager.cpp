#include "WifiManager.h"
#include "web/portal_html.h"
#include "web/portal_css.h"
#include "web/portal_js.h"

void WifiManager::begin() {
    Serial.println("[WifiManager] begin()");
    loadCredentials();
    if (_ssid.length() == 0) {
        Serial.println("[WifiManager] keine Credentials gespeichert");
    } else if (tryConnect()) {
        Serial.printf("[WifiManager] STA verbunden, IP: %s\n",
                      WiFi.localIP().toString().c_str());
        return;
    } else {
        Serial.println("[WifiManager] STA-Verbindung fehlgeschlagen");
    }
    startFallbackAp();
}

void WifiManager::loop() {
    if (!_apActive) return;
    _dns.processNextRequest();
    _server.handleClient();
    if (_portalState == PORTAL_CONNECTING) pollConnect();
    if (_portalState == PORTAL_CONNECTED && _connectedAt > 0 &&
        millis() - _connectedAt >= _apOffDelay) {
        Serial.println("[WifiManager] AP nach 30s abgeschaltet, nur noch STA");
        _server.stop();
        _dns.stop();
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        _apActive = false;
    }
}

bool WifiManager::isConnected() { return WiFi.status() == WL_CONNECTED; }
bool WifiManager::isApActive()   { return _apActive; }

void WifiManager::loadCredentials() {
    _prefs.begin("wifi", false);
    _ssid = _prefs.getString("ssid", "");
    _pass = _prefs.getString("pass", "");
    _prefs.end();
    Serial.printf("[WifiManager] Credentials geladen: ssid='%s' pass=(%u Zeichen)\n",
                  _ssid.c_str(), _pass.length());
}

bool WifiManager::tryConnect() {
    Serial.printf("[WifiManager] versuche STA mit '%s' ... ", _ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid.c_str(), _pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < _connectTimeout) delay(100);
    if (WiFi.status() == WL_CONNECTED) { Serial.println("OK"); return true; }
    Serial.println("TIMEOUT"); return false;
}

void WifiManager::startFallbackAp() {
    Serial.println("[WifiManager] starte Fallback-AP (AP+STA)");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("DoorSetup-AP");
    _apActive = true;
    _portalState = PORTAL_IDLE;
    Serial.printf("[WifiManager] AP aktiv, IP: %s\n",
                  WiFi.softAPIP().toString().c_str());
    startPortal();
}

void WifiManager::startPortal() {
    Serial.println("[WifiManager] starte Captive Portal");
    _dns.start(_dnsPort, "*", WiFi.softAPIP());
    _server.on("/",            HTTP_GET,  [this](){ handleRoot();    });
    _server.on("/portal.css",  HTTP_GET,  [this](){ handleCss();     });
    _server.on("/portal.js",   HTTP_GET,  [this](){ handleJs();      });
    _server.on("/scan",        HTTP_GET,  [this](){ handleScan();    });
    _server.on("/save",        HTTP_POST, [this](){ handleSave();   });
    _server.on("/status",      HTTP_GET,  [this](){ handleStatus();  });
    const char* captivePaths[] = {
        "/generate_204", "/gen_204", "/hotspot-detect.html",
        "/ncsi.txt", "/connecttest.txt", "/redirect"
    };
    for (auto p : captivePaths) {
        _server.on(p, HTTP_GET, [this](){ handleRedirect(); });
    }
    _server.onNotFound(                   [this](){ handleRedirect(); });
    _server.begin();
    Serial.println("[WifiManager] Portal bereit");
}

void WifiManager::pollConnect() {
    if (WiFi.status() == WL_CONNECTED) {
        _portalState = PORTAL_CONNECTED;
        _connectedAt = millis();
        Serial.printf("[WifiManager] verbunden, STA-IP: %s\n",
                      WiFi.localIP().toString().c_str());
        return;
    }
    if (millis() - _connectStart >= _connectTimeout) {
        _portalState = PORTAL_FAILED;
        Serial.println("[WifiManager] STA-Verbindung Timeout");
    }
}

void WifiManager::handleRoot() {
    Serial.println("[HTTP] GET /");
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "text/html", PORTAL_HTML);
}
void WifiManager::handleCss() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "text/css", PORTAL_CSS);
}
void WifiManager::handleJs() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "application/javascript", PORTAL_JS);
}

void WifiManager::handleScan() {
    Serial.println("[HTTP] GET /scan");
    int n = WiFi.scanNetworks(false, true);
    String json = "[";
    for (int i = 0; i < n; ++i) {
        if (i) json += ",";
        json += "{\"ssid\":\"" + WiFi.SSID(i) +
                "\",\"rssi\":" + String(WiFi.RSSI(i)) +
                ",\"secure\":" + String(WiFi.encryptionType(i) != WIFI_AUTH_OPEN ? "true" : "false") + "}";
    }
    json += "]";
    WiFi.scanDelete();
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
    Serial.printf("[HTTP] /scan -> %d Netzwerke\n", n);
}

void WifiManager::handleSave() {
    String ssid = _server.arg("ssid");
    String pass = _server.arg("pass");
    Serial.printf("[HTTP] POST /save ssid='%s'\n", ssid.c_str());
    if (ssid.length() == 0) { _server.send(400, "text/plain", "SSID fehlt"); return; }

    _prefs.begin("wifi", false);
    _prefs.putString("ssid", ssid);
    _prefs.putString("pass", pass);
    _prefs.end();

    _ssid = ssid; _pass = pass;
    _portalState = PORTAL_CONNECTING;
    _connectStart = millis();
    WiFi.begin(ssid.c_str(), pass.c_str());
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", "{\"status\":\"connecting\"}");
}

void WifiManager::handleStatus() {
    String state;
    switch (_portalState) {
        case PORTAL_CONNECTING: state = "connecting"; break;
        case PORTAL_CONNECTED:  state = "connected";  break;
        case PORTAL_FAILED:     state = "failed";     break;
        default:                state = "idle";
    }
    String ip = (_portalState == PORTAL_CONNECTED) ? WiFi.localIP().toString() : "";
    String json = "{\"state\":\"" + state + "\",\"ip\":\"" + ip + "\"}";
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
}

void WifiManager::handleRedirect() {
    Serial.printf("[HTTP] Redirect %s -> /\n", _server.uri().c_str());
    _server.sendHeader("Location", "/");
    _server.send(302, "text/plain", "");
}