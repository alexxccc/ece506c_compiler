#include <Arduino>

void setup() {
    int sensor = analogRead(0);
    bool ready = true;

    if (sensor > 10 && ready) {
        Serial.println("sensor ready");
    }
}

void loop() {
    int value;
    value = digitalRead(2) + 1;

    while (value < 4) {
        value = value + 1;
    }
}
