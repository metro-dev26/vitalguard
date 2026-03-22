#include <Wire.h>

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    int speeds[] = {100000, 50000, 10000};
    
    for (int s = 0; s < 3; s++) {
        Wire.begin(21, 22);
        Wire.setClock(speeds[s]);
        delay(500);
        
        Serial.print("Scanning at ");
        Serial.print(speeds[s]);
        Serial.println(" Hz...");
        
        for (byte i = 1; i < 127; i++) {
            Wire.beginTransmission(i);
            if (Wire.endTransmission() == 0) {
                Serial.print("Found: 0x");
                Serial.println(i, HEX);
            }
        }
        Wire.end();
        delay(500);
    }
    Serial.println("Done.");
}

void loop() {}
