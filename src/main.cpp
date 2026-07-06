#include <Arduino.h>
#include "WifiManager.h"
#include "WebInterface.h"
#include "config.h"

WifiManager wifi;
WebInterface web;
bool webStarted = false;

void setup() {
    Serial.begin(115200);
    wifi.begin();
}

void loop() {
    wifi.loop();
    if (!webStarted && wifi.isConnected() && !wifi.isApActive()) {
        web.begin(wifi);
        webStarted = true;
    }
    if (webStarted) web.loop();
}