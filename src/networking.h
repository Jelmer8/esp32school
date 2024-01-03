#ifndef NETWORKING_H
#define NETWORKING_H

void setupNetworking();
void publishData(const char* topic, const char* data, unsigned int dataLength);
void networkingLoop();

#endif
