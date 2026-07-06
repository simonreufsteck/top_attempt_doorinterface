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
    String getHostname() { return _hostname; }
    bool setHostname(const String& hostname);

private:
    Preferences _prefs;
    bool _apActive = false;
    String _ssid;
    String _pass;
    String _hostname;

    DNSServer _dns;
    WebServer _server{80};
    const byte _dnsPort = 53;

    PortalState _portalState = PORTAL_IDLE;
    unsigned long _connectStart = 0;
    const unsigned long _connectTimeout = 15000;
    unsigned long _connectedAt = 0;
    const unsigned long _apOffDelay = 30000;
    bool _shutdownRequested = false;

    void loadCredentials();
    void loadHostname();
    bool tryConnect();
    void startFallbackAp();
    void startPortal();
    void pollConnect();
    void shutdownAp();

    void handleRoot();
    void handleCss();
    void handleJs();
    void handleScan();
    void handleSave();
    void handleStatus();
    void handleConfig();
    void handleRedirect();
    void handleClose();
};