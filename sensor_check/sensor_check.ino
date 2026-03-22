void setup() {
    Serial.begin(115200);
    Serial.println("GSR Test Starting...");
}

void loop() {
    int gsrValue = analogRead(4);
    Serial.print("GSR: ");
    Serial.println(gsrValue);
    delay(100);
}