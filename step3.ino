#define N 50

float buffer[N];
int idx = 0;

float t = 0;

// -------- EEG SIGNAL GENERATION --------
float getEEG() {
    float signal = sin(2 * 3.14 * 6 * t);
    float noise = random(-100, 100) / 100.0;
    t += 0.02;
    return signal + noise;
}

// -------- FILTER --------
float filter(float val) {
    static float buf[5] = {0};
    static int i = 0;

    buf[i] = val;
    i = (i + 1) % 5;

    float sum = 0;
    for (int j = 0; j < 5; j++) {
        sum += buf[j];
    }

    return sum / 5;
}

// -------- SETUP --------
void setup() {
    Serial.begin(115200);
}

// -------- LOOP --------
void loop() {

    float raw = getEEG();
    float clean = filter(raw);

    buffer[idx++] = clean;

    Serial.print("Raw:");
Serial.print(raw);
Serial.print(" ");
Serial.print("Filtered:");
Serial.println(clean);

    if (idx >= N) {

        Serial.println("BUFFER FULL");

        idx = 0;
    }

    delay(50);
}
