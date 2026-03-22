#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

PulseOximeter pox;
uint32_t tsLastReport = 0;

void onBeatDetected() {
    Serial.println("Beat detected!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("VitalGuard Phase 1 - Initializing...");
    Wire.begin(22,21);

    if (!pox.begin()) {
        Serial.println("ERROR: MAX30100 not found!");
        Serial.println("Check wiring:");
        Serial.println("  SDA -> GPIO 21");
        Serial.println("  SCL -> GPIO 22");
        Serial.println("  VIN -> 3V3");
        Serial.println("  GND -> GND");
        while(true);
    }

    Serial.println("MAX30100 found! Sensor ready.");
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
    pox.update();

    if (millis() - tsLastReport > 1000) {
        Serial.print("Heart Rate: ");
        Serial.print(pox.getHeartRate());
        Serial.print(" bpm | SpO2: ");
        Serial.print(pox.getSpO2());
        Serial.println("%");
        tsLastReport = millis();
    }
}
