#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <DNSServer.h>
#include <WebServer.h>

enum PortalState { PORTAL_IDLE, PORTAL_CONNECTING, PORTAL_CONNECTED, PORTAL_FAILED };

class WifiManager {
public:
    void begin();
    void loop();
    bool isConnected();
    bool isApActive();

private:
    Preferences _prefs;
    bool _apActive = false;
    String _ssid;
    String _pass;

    DNSServer _dns;
    WebServer _server{80};
    const byte _dnsPort = 53;

    PortalState _portalState = PORTAL_IDLE;
    unsigned long _connectStart = 0;
    const unsigned long _connectTimeout = 15000;
    unsigned long _connectedAt = 0;
    const unsigned long _apOffDelay = 30000;

    void loadCredentials();
    bool tryConnect();
    void startFallbackAp();
    void startPortal();
    void pollConnect();

    void handleRoot();
    void handleCss();
    void handleJs();
    void handleScan();
    void handleSave();
    void handleStatus();
    void handleRedirect();
};