#include <HardwareSerial.h>
#include "networking.h"
#include "doorfeature.h"
#include "movementfeature.h"

void setup() {
    Serial.begin(115200);
    setupNetworking();
    setupDoorFeature();
    setupMovementFeature();
    //publishData("jelmerdejong/test", "test123", 7);
}

void loop() {
    networkingLoop();
    doorFeatureLoop();
    movementFeatureLoop();
}