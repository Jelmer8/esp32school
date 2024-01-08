#include <Arduino.h>

#define MOVEMENT_SENSOR_PIN 27
#define RELAY_PIN 14

// CONSTANTS

constexpr int timeout = 60000 ; // Light will stay on x milliseconds after movement stops


// VARIABLES

int pinStatePrevious  = LOW; // The previous state of the pin
unsigned long lastMotionAt = 0;
bool relayOn = false;


void setupMovementFeature() {
    pinMode(MOVEMENT_SENSOR_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
}

void movementFeatureLoop() {
    const int pinStateCurrent = digitalRead(MOVEMENT_SENSOR_PIN);

    if (pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
        Serial.println("Motion detected!");
        digitalWrite(RELAY_PIN, HIGH);
        lastMotionAt = millis();
        relayOn = true;
        // TODO: turn on alarm, light or activate a device ... here
    } else if (pinStateCurrent == LOW && lastMotionAt + timeout < millis() && relayOn) {   // pin state change: HIGH -> LOW
        Serial.println("Motion stopped for 'timeout' milliseconds!");
        digitalWrite(RELAY_PIN, LOW);
        relayOn = false;
        // TODO: turn off alarm, light or deactivate a device ... here
    } else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {
        Serial.println("Motion stopped");
    }

    pinStatePrevious = pinStateCurrent; // store old state
}