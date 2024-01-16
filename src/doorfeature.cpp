#include <Arduino.h>
#include "networking.h"

using namespace std;

// CONSTANTS
constexpr unsigned int TRIG_PIN = 33;
constexpr unsigned int ECHO_PIN = 32;
constexpr unsigned int TRANSISTOR_PIN = 25;
constexpr unsigned int DISTANCE_DOOR = 10; // Distance from the sensor to the door when door is shut
constexpr long BUZZ_DELAY = 2000; // Start buzzing after the door is opened for more than x milliseconds
constexpr int INTERVAL = 50; // In milliseconds

// VARIABLES
static unsigned long previousMillis = 0;
bool turnOffBuzzer = false; // True if the buzzer was on but now needs to be turned off
long doorOpenTimeStamp = 0;
bool doorCurrentlyOpen = false;
static bool started = false;

void resumeDoorMsAgo(const unsigned long msAgo) {
    if (started) { // There's no need to resume anymore
        return;
    }

    doorOpenTimeStamp = -msAgo;
    if (msAgo > BUZZ_DELAY && doorCurrentlyOpen) {
        digitalWrite(TRANSISTOR_PIN, HIGH);
        turnOffBuzzer = true;
    } // Else is not needed, buzzer does not turn on when ESP turns on
}

void resumeDoorState(const int state) {
    if (started) { // There's no need to resume anymore
        return;
    }

    doorCurrentlyOpen = state;
    if (doorCurrentlyOpen && doorOpenTimeStamp + BUZZ_DELAY < static_cast<long>(millis())) {
        digitalWrite(TRANSISTOR_PIN, HIGH);
        turnOffBuzzer = true;
    } // Else is not needed, buzzer does not turn on when ESP turns on
}


void setupDoorFeature() {
    pinMode(TRIG_PIN, OUTPUT); // Set pin modes to in/output
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRANSISTOR_PIN, OUTPUT);
}

void doorFeatureLoop() {
    if (previousMillis + INTERVAL > millis())
        return; // If less time than 'INTERVAL' has passed, return to ensure we dont run too often

    digitalWrite(TRIG_PIN, LOW); // Send out an ultrasonic pulse
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    const unsigned long duration = pulseIn(ECHO_PIN, HIGH); // Listen for ultrasonic pulse
    const unsigned long distance = duration/29/2; // Calcul

    if (duration == 0) {
        // Sensor did not receive pulse
        doorCurrentlyOpen = false;
    } else {
        // Sensor did receive pulse
        if (distance > DISTANCE_DOOR) { // Door is definitely opened
            if (!doorCurrentlyOpen) { // Door was not yet opened before this loop
                doorOpenTimeStamp = millis(); // Set timestamp to time when the open door was first discovered
                doorCurrentlyOpen = true;

                publishData("jelmerdejong/doorOpenMsAgo", "0", 1);
                    // Publish the data
                publishData("jelmerdejong/doorCurrentlyOpen", "1", 1);

                return; // Return, we dont need to check if door has been open long enough, it hasn't
            }

            if (doorOpenTimeStamp + BUZZ_DELAY < millis() && doorCurrentlyOpen) {
                // Door has been opened for more than BUZZ_DELAY milliseconds, turn on the buzzer
                digitalWrite(TRANSISTOR_PIN, HIGH);
                turnOffBuzzer = true;

                const unsigned long msAgo = millis()-doorOpenTimeStamp; // Calculate how long the door has been open for
                const char* msAgoStr = to_string(msAgo).c_str(); // Convert number to text

                publishData("jelmerdejong/doorOpenMsAgo", msAgoStr, strlen(msAgoStr));

            }
        } else { // Door is definitely shut
            doorCurrentlyOpen = false;

            started = true;

            if (turnOffBuzzer) { // If buzzer is still on, turn it off
                digitalWrite(TRANSISTOR_PIN, LOW);
                turnOffBuzzer = false;
                publishData("jelmerdejong/doorCurrentlyOpen", "0", 1);
            }
        }
    }

    previousMillis = millis();
}