#ifndef NETWORKING_H
#define NETWORKING_H
#include <Arduino.h>

void setupNetworking();
void publishData(const char* topic, const char* data, int dataLength);

#endif
