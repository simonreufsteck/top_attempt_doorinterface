#include <Arduino.h>
#include "WifiManager.h"
#include "WebInterface.h"
#include "NukiManager.h"
#include "config.h"

WifiManager wifi;
WebInterface web;
NukiManager nuki;
bool webStarted = false;

void setup() {
    Serial.begin(115200);
    wifi.begin();
    nuki.begin();
}

void loop() {
    wifi.loop();
    nuki.loop();
    if (!webStarted && wifi.isConnected() && !wifi.isApActive()) {
        web.begin(wifi, nuki);
        webStarted = true;
    }
    if (webStarted) web.loop();
}
