#include <cstring>
#include <HardwareSerial.h>
#include "networking.h"
#include "doorfeature.h"

void setup() {
    Serial.begin(115200);
    setupNetworking();
    publishData("jelmerdejong/test", "test123", 7);
}

void loop() {
    networkingLoop();
    doorFeatureLoop();
}