#ifndef DOORFEATURE_H
#define DOORFEATURE_H

extern unsigned long doorOpenTimeStamp;
extern bool doorCurrentlyOpen;

void setupDoorFeature();
void doorFeatureLoop();

#endif //DOORFEATURE_H
