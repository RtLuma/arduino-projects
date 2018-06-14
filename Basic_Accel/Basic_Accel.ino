#include <SlowSoftI2CMaster.h>
#include "MPU6050.h"

SlowSoftI2CMaster si = SlowSoftI2CMaster(23, 22, true);


void ACCELEROMETER_ISR() {
  if (!si.i2c_start((MPU6050_ADDRESS_AD0_LOW << 1) | I2C_WRITE)) {
    Serial.println("I2C device busy");
    return;
  }
  si.i2c_write(MPU6050_RA_ACCEL_XOUT_H);
  si.i2c_rep_start((MPU6050_ADDRESS_AD0_LOW << 1) | I2C_READ);
  int16_t AcX = si.i2c_read(false) << 8 | si.i2c_read(false);
  int16_t AcY = si.i2c_read(false) << 8 | si.i2c_read(true);
  si.i2c_stop();

  if (Serial.available()) {
    Serial.print(AcX);
    Serial.print("\t");
    Serial.print(AcY);
    Serial.print("\t");
    Serial.print(AcX / 4000);
    Serial.print("\t");
    Serial.println(AcY / 4000);
  }
}

void setup(void) {
  Serial.begin(38400);
  pinMode(12, INPUT);
  if (!si.i2c_init()) Serial.println("I2C init failed");

  // Wake up
  si.i2c_start((MPU6050_ADDRESS_AD0_LOW << 1) | I2C_WRITE);
  si.i2c_write(MPU6050_RA_PWR_MGMT_1);
  si.i2c_write(0);
  si.i2c_stop();

  // Enable interupts
  si.i2c_start((MPU6050_ADDRESS_AD0_LOW << 1) | I2C_WRITE);
  si.i2c_write(MPU6050_RA_INT_ENABLE);
  si.i2c_write(0x32);
  si.i2c_stop();

  // Enable DMP interupt
  si.i2c_start((MPU6050_ADDRESS_AD0_LOW << 1) | I2C_WRITE);
  si.i2c_write(MPU6050_RA_USER_CTRL);
  si.i2c_write(1 << MPU6050_USERCTRL_DMP_EN_BIT);
  si.i2c_stop();

  // Attach inerupt to digital pin leading to INT
  attachInterrupt(digitalPinToInterrupt(12), ACCELEROMETER_ISR, RISING);

}

void loop(void) {

}
