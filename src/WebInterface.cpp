#include "WebInterface.h"
#include "WifiManager.h"
#include "NukiManager.h"
#include "web/main_html.h"
#include "web/main_css.h"
#include "web/main_js.h"
#include "web/setup_html.h"
#include "web/setup_js.h"
#include "config.h"

void WebInterface::begin(WifiManager& wifi, NukiManager& nuki) {
    _wifi = &wifi;
    _nuki = &nuki;
    String hostname = _wifi->getHostname();
    if (MDNS.begin(hostname.c_str())) MDNS.addService("http", "tcp", 80);
    _server.on("/",             HTTP_GET,  [this](){ handleRoot();        });
    _server.on("/main.css",     HTTP_GET,  [this](){ handleCss();         });
    _server.on("/main.js",      HTTP_GET,  [this](){ handleJs();          });
    _server.on("/setup",        HTTP_GET,  [this](){ handleSetup();       });
    _server.on("/setup.js",     HTTP_GET,  [this](){ handleSetupJs();     });
    _server.on("/api/status",   HTTP_GET,  [this](){ handleStatus();      });
    _server.on("/api/hostname", HTTP_GET,  [this](){ handleHostnameGet(); });
    _server.on("/api/hostname", HTTP_POST, [this](){ handleHostnamePost();});
    _server.on("/api/nuki/pair",   HTTP_POST, [this](){ handleNukiPair();   });
    _server.on("/api/nuki/cancel", HTTP_POST, [this](){ handleNukiCancel(); });
    _server.on("/api/nuki/unlock", HTTP_POST, [this](){ handleNukiUnlock(); });
    _server.on("/api/nuki/lock",   HTTP_POST, [this](){ handleNukiLock();   });
    _server.onNotFound(                     [this](){ handleNotFound();   });
    _server.begin();
    Serial.printf("[HTTP] Hauptseite bereit (%s.local)\n", hostname.c_str());
}

void WebInterface::loop() {
    if (_restartRequested && millis() >= _restartAt) {
        Serial.println("[HTTP] Neustart (Hostname geaendert)");
        delay(100);
        ESP.restart();
    }
    _server.handleClient();
}

void WebInterface::handleRoot() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "text/html", MAIN_HTML);
}
void WebInterface::handleCss() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "text/css", MAIN_CSS);
}
void WebInterface::handleJs() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "application/javascript", MAIN_JS);
}
void WebInterface::handleSetup() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "text/html", SETUP_HTML);
}
void WebInterface::handleSetupJs() {
    _server.sendHeader("Cache-Control", "no-store");
    _server.send_P(200, "application/javascript", SETUP_JS);
}
void WebInterface::handleNotFound() {
    _server.send(404, "text/plain", "Not found");
}
void WebInterface::handleStatus() {
    bool connected = WiFi.status() == WL_CONNECTED;
    String json = "{";
    json += "\"wifi\":{\"connected\":" + String(connected ? "true" : "false");
    json += ",\"ssid\":\"" + WiFi.SSID() + "\"";
    json += ",\"rssi\":" + String(WiFi.RSSI());
    json += ",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    json += ",\"relay\":{\"available\":false}";
    bool paired = _nuki->isPaired();
    json += ",\"locks\":{\"available\":" + String(paired ? "true" : "false");
    json += ",\"count\":" + String(paired ? "1" : "0");
    json += ",\"paired\":" + String(paired ? "true" : "false");
    json += ",\"pairing\":" + String(_nuki->isPairing() ? "true" : "false");
    if (paired) {
        json += ",\"lockState\":\"" + _nuki->getLockStateStr() + "\"";
        json += ",\"batteryPct\":" + String(_nuki->getBatteryPct());
        json += ",\"batteryCritical\":" + String(_nuki->isBatteryCritical() ? "true" : "false");
        json += ",\"rssi\":" + String(_nuki->getRssi());
    }
    json += "}";
    json += ",\"firmware\":{\"version\":\"" FW_VERSION "\"}";
    json += "}";
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
}
void WebInterface::handleHostnameGet() {
    String json = "{\"hostname\":\"" + _wifi->getHostname() + "\"}";
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", json);
}
void WebInterface::handleHostnamePost() {
    String hostname = _server.arg("hostname");
    if (_wifi->setHostname(hostname)) {
        _server.sendHeader("Cache-Control", "no-store");
        _server.send(200, "application/json", "{\"ok\":true}");
        _restartRequested = true;
        _restartAt = millis() + 1000;
    } else {
        _server.send(400, "application/json", "{\"error\":\"ungueltiger Hostname\"}");
    }
}
void WebInterface::handleNukiPair() {
    if (_nuki->isPaired()) {
        _server.send(200, "application/json", "{\"status\":\"already_paired\"}");
        return;
    }
    _nuki->startPairing();
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", "{\"status\":\"pairing\"}");
}
void WebInterface::handleNukiCancel() {
    _nuki->cancelPairing();
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", "{\"status\":\"cancelled\"}");
}
void WebInterface::handleNukiUnlock() {
    if (!_nuki->isPaired()) {
        _server.send(409, "application/json", "{\"ok\":false,\"error\":\"not_paired\"}");
        return;
    }
    bool ok = _nuki->unlock();
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", ok ? "{\"ok\":true}" : "{\"ok\":false,\"error\":\"failed\"}");
}
void WebInterface::handleNukiLock() {
    if (!_nuki->isPaired()) {
        _server.send(409, "application/json", "{\"ok\":false,\"error\":\"not_paired\"}");
        return;
    }
    bool ok = _nuki->lock();
    _server.sendHeader("Cache-Control", "no-store");
    _server.send(200, "application/json", ok ? "{\"ok\":true}" : "{\"ok\":false,\"error\":\"failed\"}");
}
