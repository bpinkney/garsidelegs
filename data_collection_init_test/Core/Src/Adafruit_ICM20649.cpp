/*!   @file Adafruit_ICM20649.cpp
 */

#include "Adafruit_ICM20649.h"
#include "Adafruit_ICM20X.h"


/*void Adafruit_ICM20649_scaleValues(void) {

  icm20649_gyro_range_t gyro_range = (icm20649_gyro_range_t)current_gyro_range;
  icm20649_accel_range_t accel_range =
      (icm20649_accel_range_t)current_accel_range;
  float accel_scale = 1.0;
  float gyro_scale = 1.0;

  if (gyro_range == ICM20649_GYRO_RANGE_500_DPS)
    gyro_scale = 65.5;
  if (gyro_range == ICM20649_GYRO_RANGE_1000_DPS)
    gyro_scale = 32.8;
  if (gyro_range == ICM20649_GYRO_RANGE_2000_DPS)
    gyro_scale = 16.4;
  if (gyro_range == ICM20649_GYRO_RANGE_4000_DPS)
    gyro_scale = 8.2;

  if (accel_range == ICM20649_ACCEL_RANGE_4_G)
    accel_scale = 8192.0;
  if (accel_range == ICM20649_ACCEL_RANGE_8_G)
    accel_scale = 4096.0;
  if (accel_range == ICM20649_ACCEL_RANGE_16_G)
    accel_scale = 2048.0;
  if (accel_range == ICM20649_ACCEL_RANGE_30_G)
    accel_scale = 1024.0;

  gyroX = rawGyroX / gyro_scale;
  gyroY = rawGyroY / gyro_scale;
  gyroZ = rawGyroZ / gyro_scale;

  accX = rawAccX / accel_scale;
  accY = rawAccY / accel_scale;
  accZ = rawAccZ / accel_scale;
}*/

/**************************************************************************/
/*!
    @brief Get the accelerometer's measurement range.
    @returns The accelerometer's measurement range (`icm20649_accel_range_t`).
*/
/*icm20649_accel_range_t Adafruit_ICM20649::getAccelRange(void) {
  return (icm20649_accel_range_t)readAccelRange();
}*/

/**************************************************************************/
/*!

    @brief Sets the accelerometer's measurement range.
    @param  new_accel_range
            Measurement range to be set. Must be an
            `icm20649_accel_range_t`.
*/
/*void Adafruit_ICM20649::setAccelRange(icm20649_accel_range_t new_accel_range) {
  writeAccelRange((uint8_t)new_accel_range);
}*/

/**************************************************************************/
/*!
    @brief Get the gyro's measurement range.
    @returns The gyro's measurement range (`icm20649_gyro_range_t`).
*/
/*icm20649_gyro_range_t Adafruit_ICM20649::getGyroRange(void) {
  return (icm20649_gyro_range_t)readGyroRange();
}*/

/**************************************************************************/
/*!

    @brief Sets the gyro's measurement range.
    @param  new_gyro_range
            Measurement range to be set. Must be an
            `icm20649_gyro_range_t`.
*/
/*void Adafruit_ICM20649::setGyroRange(icm20649_gyro_range_t new_gyro_range) {
  writeGyroRange((uint8_t)new_gyro_range);
}*/
