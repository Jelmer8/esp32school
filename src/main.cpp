#include <cstring>
#include <HardwareSerial.h>
#include "networking.h"

void setup() {
    Serial.begin(115200);
    setupNetworking();
    publishData("jelmerdejong/test", "test123", 7);
}

void loop() {

}