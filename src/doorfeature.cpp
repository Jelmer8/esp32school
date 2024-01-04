#include <Arduino.h>
#include "networking.h"
#include "doorfeature.h"

using namespace std;

// CONSTANTS
constexpr unsigned int TRIG_PIN = 33;
constexpr unsigned int ECHO_PIN = 32;
constexpr unsigned int TRANSISTOR_PIN = 25;
constexpr unsigned int DISTANCE_DOOR = 10; // Distance from the sensor to the door when door is shut

// VARIABLES
unsigned long previousMillis = 0;
unsigned long interval = 500; // In milliseconds
unsigned long buzzDelay = 2000; // Start buzzing after the door is opened for more than x milliseconds
bool turnOffBuzzer = false; // True if the buzzer was on but now needs to be turned off
unsigned long doorOpenTimeStamp = 0;
bool doorCurrentlyOpen = false;


void setupDoorFeature() {
    pinMode(TRIG_PIN, OUTPUT); // Set pin modes to in/output
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRANSISTOR_PIN, OUTPUT);
}

//TODO: Resume from MQTT Values

void doorFeatureLoop() {
    if (previousMillis + interval > millis())
        return; // If less time than 'interval' has passed, return to ensure we dont run too often

    digitalWrite(TRIG_PIN, LOW); // Send out an ultrasonic pulse
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    const unsigned long duration = pulseIn(ECHO_PIN, HIGH); // Listen for ultrasonic pulse
    const unsigned long distance = duration/29/2;

    if (duration == 0) {
        // Sensor did not receive pulse
        doorCurrentlyOpen = false;
    } else {
        // Sensor did receive pulse
        if (distance > DISTANCE_DOOR) { // Door is definitely opened
            if (!doorCurrentlyOpen) { // Door was not yet opened before this loop
                doorOpenTimeStamp = millis(); // Set timestamp to time when the open door was first discovered
                doorCurrentlyOpen = true;

                const char* timestamp = to_string(millis()).c_str(); // Convert timestamp to string
                publishData("jelmerdejong/doorOpenTimestamp", timestamp, strlen(timestamp));
                    // Publish the data
                publishData("jelmerdejong/doorCurrentlyOpen", "1", 1);

                return; // Return, we dont need to check if door has been open long enough, it hasn't
            }

            if (doorOpenTimeStamp + buzzDelay < millis() && doorCurrentlyOpen) {
                // Door has been opened for more than buzzDelay milliseconds, turn on the buzzer
                digitalWrite(TRANSISTOR_PIN, HIGH);
                turnOffBuzzer = true;
            }
        } else { // Door is definitely shut
            doorCurrentlyOpen = false;

            if (turnOffBuzzer) { // If buzzer is still on, turn it off
                digitalWrite(TRANSISTOR_PIN, LOW);
                turnOffBuzzer = false;
                publishData("jelmerdejong/doorCurrentlyOpen", "0", 1);
            }
        }
    }

    previousMillis = millis();
}