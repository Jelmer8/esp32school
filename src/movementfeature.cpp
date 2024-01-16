#include <Arduino.h>
#include "networking.h"

#define MOVEMENT_SENSOR_PIN 27
#define RELAY_PIN 14

using namespace std;

// CONSTANTS

constexpr int timeout = 60000 ; // Light will stay on x milliseconds after movement stops
constexpr int interval = 50; // In milliseconds


// VARIABLES

int pinStatePrevious  = LOW; // The previous state of the pin
unsigned long lastMotionAt = 0; // Timestamp of last motion detected
bool relayOn = false; // State of the relay
static unsigned long previousMillis = 0;




void resumeMotionTimestamp(unsigned long timestamp) {

}

void setupMovementFeature() {
    pinMode(MOVEMENT_SENSOR_PIN, INPUT); // Set pin modes to in/output
    pinMode(RELAY_PIN, OUTPUT);
}

void movementFeatureLoop() {
    if (previousMillis + interval > millis())
        return; // If less time than 'interval' has passed, return to ensure we dont run too often

    const int pinStateCurrent = digitalRead(MOVEMENT_SENSOR_PIN); // Get pin reading

    if (pinStateCurrent == HIGH) { // If movement was detected
        Serial.println("Motion detected!");
        lastMotionAt = millis(); // Set timestamp to time when last motion was detected
        const char* timestamp = to_string(lastMotionAt).c_str(); // Convert timestamp to string
        publishData("jelmerdejong/lastMotionTimestamp", timestamp, strlen(timestamp)); // Send timestamp to MQTT-server
        digitalWrite(RELAY_PIN, HIGH); // Turn on relay
        relayOn = true; // Save relay state
    } else if (pinStateCurrent == LOW && lastMotionAt + timeout < millis() && relayOn) { // If movement was not detected and the relay is still on and should be turned off
        Serial.println("Motion stopped for 'timeout' milliseconds!");
        digitalWrite(RELAY_PIN, LOW); // Turn off relay
        relayOn = false; // Save relay state
    } else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) { // If movement was detected previously, but not anymore
        Serial.println("Motion stopped");
    }

    pinStatePrevious = pinStateCurrent; // Store old state
    previousMillis = millis();
}