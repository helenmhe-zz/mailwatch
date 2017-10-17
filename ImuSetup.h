#ifndef ImuSetup_h
#define ImuSetup_h
#include "Arduino.h"
#include <MPU9250.h>

void setup_angle(MPU9250 &imu);
void get_angle(float&x, float&y, MPU9250 &imu);

#endif
