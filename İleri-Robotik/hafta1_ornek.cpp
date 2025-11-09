#include "lsm6dsm.h"

LSM6DSM IMU;

void setup() {
  Serial.begin(115200);
  IMU.begin();
}

void loop() {
  float x_ekseni_ivme, y_ekseni_ivme;

  float x_ekseni = IMU.readFloatAccelX();
  float y_ekseni = IMU.readFloatAccelY();
  float z_ekseni = IMU.readFloatAccelZ();

  x_ekseni_ivme = atan(y_ekseni / sqrt(pow(z_ekseni, 2) + pow(z_ekseni, 2) + 0.001)) * 180 / PI;
  y_ekseni_ivme = atan(-1 * x_ekseni / sqrt(pow(y_ekseni, 2) + pow(z_ekseni, 2) + 0.001)) * 180 / PI;

  Serial.print("X Ekseni İvme: ");
  Serial.println(x_ekseni_ivme);
  Serial.print("Y Ekseni İvme: ");
  Serial.println(y_ekseni_ivme);

  delay(500);
}