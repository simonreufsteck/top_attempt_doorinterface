#include <Arduino.h>
#include "WifiManager.h"

WifiManager wifi;

void setup() {
    Serial.begin(115200);
    wifi.begin();
}

void loop() {
    wifi.loop();
}