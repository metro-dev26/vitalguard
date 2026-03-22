#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MAX30100_PulseOximeter.h"

// WiFi credentials
const char* ssid     = "Airtel_MAHIKA";
const char* password = "chbh@7078";

// Backend URL
const char* serverURL = "http://192.168.1.41:8000/ingest";

PulseOximeter pox;
uint32_t tsLastSend = 0;

void onBeatDetected() {
    Serial.println("Beat!");
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

    // Initialize sensor
    Wire.begin(21, 22);
    Wire.setClock(100000);
    delay(500);

    if (!pox.begin()) {
        Serial.println("ERROR: Sensor not found!");
        while(true);
    }

    pox.setIRLedCurrent(MAX30100_LED_CURR_50MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
    Serial.println("Sensor ready. Place finger.");
}

void loop() {
    pox.update();

    // Send data every 40ms (25Hz)
    if (millis() - tsLastSend >= 40) {

        // Read raw IR from register
        Wire.beginTransmission(0x57);
        Wire.write(0x05);
        Wire.endTransmission(false);
        Wire.requestFrom(0x57, 4);

        uint16_t ir = 0, red = 0;
        if (Wire.available() >= 4) {
            ir  = (Wire.read() << 8) | Wire.read();
            red = (Wire.read() << 8) | Wire.read();
        }

        float hr   = pox.getHeartRate();
        float spo2 = pox.getSpO2();

        // Build JSON
        String json = "{\"ir\":" + String(ir) +
                      ",\"red\":" + String(red) +
                      ",\"hr\":" + String(hr) +
                      ",\"spo2\":" + String(spo2) + "}";

        // Send to FastAPI
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverURL);
            http.addHeader("Content-Type", "application/json");
            int responseCode = http.POST(json);
            Serial.println("Sent: " + json + " | Response: " + String(responseCode));
            http.end();
        }

        tsLastSend = millis();
    }
}