float t = 0;

float getEEG() {
    float signal = sin(2 * 3.14 * 6 * t);
    float noise = random(-50, 50) / 100.0;
    t += 0.02;
    return signal + noise;
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    float raw = getEEG();
    Serial.println(raw);
    delay(50);
}
