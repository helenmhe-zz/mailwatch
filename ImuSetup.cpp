#include "Arduino.h"
#include <MPU9250.h>
#include "ImuSetup.h"

void setup_angle(MPU9250 &imu) {
  char c = imu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  Serial.print("MPU9250: "); Serial.print("I AM "); Serial.print(c, HEX);
  Serial.print(" I should be "); Serial.println(0x73, HEX);
  if (c == 0x73) {
    imu.MPU9250SelfTest(imu.selfTest);
    imu.initMPU9250();
    imu.calibrateMPU9250(imu.gyroBias, imu.accelBias);
    imu.initMPU9250();
    imu.initAK8963(imu.factoryMagCalibration);
  } 
  else
  {
    while (1) Serial.println("NOT FOUND"); // Loop forever if communication doesn't happen
  }
  imu.getAres();
  imu.getGres();
  imu.getMres();
}

void get_angle(float&x, float&y, MPU9250 &imu) {
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
}

