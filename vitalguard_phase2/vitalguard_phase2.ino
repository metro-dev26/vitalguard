#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

PulseOximeter pox;

uint16_t irValue = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Wire.begin(21, 22);
    Wire.setClock(100000);
    delay(500);

    if (!pox.begin()) {
        Serial.println("ERROR");
        while(true);
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_50MA);
    Serial.println("READY");
}

void loop() {
    pox.update();

    static uint32_t lastSample = 0;
    if (millis() - lastSample >= 40) {

        // Read IR directly from register
        Wire.beginTransmission(0x57);
        Wire.write(0x05);
        Wire.endTransmission(false);
        Wire.requestFrom(0x57, 4);

        if (Wire.available() >= 4) {
            irValue  = (Wire.read() << 8) | Wire.read();
            uint16_t redValue = (Wire.read() << 8) | Wire.read();
            Serial.println(irValue);
        }

        lastSample = millis();
    }
}