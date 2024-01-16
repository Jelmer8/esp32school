#include <HardwareSerial.h>
#include "networking.h"
#include "doorfeature.h"
#include "movementfeature.h"

void setup() { // Run setup function of all features
    Serial.begin(115200);
    setupNetworking();
    setupDoorFeature();
    setupMovementFeature();
    //publishData("jelmerdejong/test", "test123", 7);
}

void loop() { // Run loop function of all features
    networkingLoop();
    doorFeatureLoop();
    movementFeatureLoop();
}