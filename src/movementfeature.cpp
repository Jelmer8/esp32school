#include <Arduino.h>
#include "networking.h"

#define MOVEMENT_SENSOR_PIN 27
#define RELAY_PIN 14

using namespace std;

// CONSTANTS

constexpr int TIMEOUT = 60000 ; // Light will stay on x milliseconds after movement stops
constexpr int INTERVAL = 50; // In milliseconds


// VARIABLES

unsigned long lastMotionAt = 0; // Timestamp of last motion detected
bool relayOn = false; // State of the relay
static unsigned long previousMillis = 0;




void resumeMotionMsAgo(const unsigned long msAgo) {
    if (msAgo < TIMEOUT) {
        digitalWrite(RELAY_PIN, HIGH); // Turn on relay
        relayOn = true; // Save relay state
    } // Else is not needed, relay does not turn on when ESP turns on
}

void setupMovementFeature() {
    pinMode(MOVEMENT_SENSOR_PIN, INPUT); // Set pin modes to in/output
    pinMode(RELAY_PIN, OUTPUT);
}

void movementFeatureLoop() {
    if (previousMillis + INTERVAL > millis())
        return; // If less time than 'INTERVAL' has passed, return to ensure we dont run too often

    const int pinStateCurrent = digitalRead(MOVEMENT_SENSOR_PIN); // Get pin reading

    if (pinStateCurrent == HIGH) { // If movement was detected
        lastMotionAt = millis(); // Set timestamp to time when last motion was detected
        publishData("jelmerdejong/lastMotionMsAgo", "0", 1); // Set lastMotionMsAgo to 0
        digitalWrite(RELAY_PIN, HIGH); // Turn on relay
        relayOn = true; // Save relay state
    } else if (pinStateCurrent == LOW && lastMotionAt + TIMEOUT < millis() && relayOn) { // If movement was not detected and the relay is still on and should be turned off
        const unsigned long msAgo = millis()-lastMotionAt; // Calculate time since last movement
        const char* msAgoStr = to_string(msAgo).c_str(); // Convert number to text

        publishData("jelmerdejong/lastMotionMsAgo", msAgoStr, strlen(msAgoStr)); // Set lastMotionMsAgo to msAgo
        digitalWrite(RELAY_PIN, LOW); // Turn off relay
        relayOn = false; // Save relay state
    }

    previousMillis = millis();
}