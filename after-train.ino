#include "seizure_model.h"

Eloquent::ML::Port::DecisionTree model;

void setup() {
    Serial.begin(115200);
    Serial.println("Model test started");
}

void loop() {
    float x[10] = {0.2, -0.5, 0.1, -0.3, 0.4, -0.2, 0.0, -0.1, 0.3, -0.4};

    int result = model.predict(x);

    Serial.print("Prediction: ");
    Serial.println(result);

    if (result == 0) {
        Serial.println("Normal");
    }
    else if (result == 1) {
        Serial.println("Pre-ictal");
    }
    else {
        Serial.println("Seizure");
    }

    delay(2000);
}
