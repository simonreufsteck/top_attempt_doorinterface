#include "WifiManager.h"
#include "web/portal_html.h"
#include "web/portal_css.h"
#include "web/portal_js.h"
#include <uri/UriGlob.h>

void WifiManager::begin() {
    Serial.println("[WifiManager] begin()");
    loadHostname();
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
    if (_apActive && _shutdownRequested) { _shutdownRequested = false; shutdownAp(); return; }
    if (!_apActive) return;
    _dns.processNextRequest();
    _server.handleClient();
    if (_portalState == PORTAL_CONNECTING) pollConnect();
    if (_portalState == PORTAL_CONNECTED && _connectedAt > 0 &&
        millis() - _connectedAt >= _apOffDelay) {
        Serial.println("[WifiManager] AP nach 30s abgeschaltet, nur noch STA");
        shutdownAp();
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

void WifiManager::loadHostname() {
    _prefs.begin("system", false);
    _hostname = _prefs.getString("hostname", "");
    if (_hostname.length() == 0) {
        uint8_t mac[6]; WiFi.macAddress(mac);
        char def[32];
        snprintf(def, sizeof(def), "doorinterface-%02x%02x", mac[4], mac[5]);
        _hostname = String(def);
        _prefs.putString("hostname", _hostname);
    }
    _prefs.end();
    Serial.printf("[WifiManager] Hostname: %s\n", _hostname.c_str());
}

bool WifiManager::setHostname(const String& hostname) {
    if (hostname.length() == 0 || hostname.length() > 63) return false;
    for (unsigned i = 0; i < hostname.length(); ++i) {
        char c = hostname[i];
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-')) return false;
    }
    if (hostname[0] == '-' || hostname[hostname.length()-1] == '-') return false;
    _hostname = hostname;
    _prefs.begin("system", false);
    _prefs.putString("hostname", hostname);
    _prefs.end();
    return true;
}

bool WifiManager::tryConnect() {
    Serial.printf("[WifiManager] versuche STA mit '%s' ... ", _ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(_hostname.c_str());
    WiFi.begin(_ssid.c_str(), _pass.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < _connectTimeout) delay(100);
    if (WiFi.status() == WL_CONNECTED) { Serial.println("OK"); return true; }
    Serial.println("TIMEOUT"); return false;
}

void WifiManager::startFallbackAp() {
    Serial.println("[WifiManager] starte Fallback-AP (AP+STA)");
    WiFi.setAutoReconnect(false);
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("DoorSetup-AP");
    WiFi.disconnect(false);
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
    _server.on("/config",      HTTP_GET,  [this](){ handleConfig();  });
    _server.on("/close",       HTTP_POST, [this](){ handleClose();  });
    _server.on(UriGlob("*"), HTTP_ANY,    [this](){ handleRedirect(); });
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
    String hostname = _server.arg("hostname");
    Serial.printf("[HTTP] POST /save ssid='%s'\n", ssid.c_str());
    if (ssid.length() == 0) { _server.send(400, "text/plain", "SSID fehlt"); return; }

    if (hostname.length() > 0) setHostname(hostname);

    _prefs.begin("wifi", false);
    _prefs.putString("ssid", ssid);
    _prefs.putString("pass", pass);
    _prefs.end();

    _ssid = ssid; _pass = pass;
    _portalState = PORTAL_CONNECTING;
    _connectStart = millis();
    WiFi.setHostname(_hostname.c_str());
    WiFi.begin(ssid.c_str(), pass.c_str());
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", "{\"status\":\"connecting\"}");
}

void WifiManager::handleConfig() {
    String json = "{\"hostname\":\"" + _hostname + "\"}";
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
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
    _server.sendHeader("Location", "/");
    _server.send(302, "text/plain", "");
}

void WifiManager::handleClose() {
    Serial.println("[HTTP] POST /close");
    if (_portalState != PORTAL_CONNECTED) {
        _server.send(409, "application/json", "{\"status\":\"not-connected\"}");
        return;
    }
    _shutdownRequested = true;
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", "{\"status\":\"closing\"}");
}

void WifiManager::shutdownAp() {
    _server.stop();
    _dns.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    _apActive = false;
    _portalState = PORTAL_IDLE;
    Serial.println("[WifiManager] AP geschlossen, nur noch STA");
}