/*!
 *  @file Adafruit_ICM20X.h
 *
 * 	I2C Driver for the Adafruit ICM20X 6-DoF Wide-Range Accelerometer and
 *Gyro library
 *
 * 	This is a library for the Adafruit ICM20X breakouts:
 * 	https://www.adafruit.com/product/4464
 * 	https://www.adafruit.com/product/4554
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_ICM20X_H
#define _ADAFRUIT_ICM20X_H

// Misc configuration macros
#define I2C_MASTER_RESETS_BEFORE_FAIL                                          \
  5 ///< The number of times to try resetting a stuck I2C master before giving
    ///< up
#define NUM_FINISHED_CHECKS                                                    \
  100 ///< How many times to poll I2C_SLV4_DONE before giving up and resetting

/**************************************************************************//**
* @name ICM20649 register banks (you can | addresses with these instead of changing banks
* @{
******************************************************************************/
#define ICM20649_BANK_0                  	((uint8_t)0 << 7)     /**< Register bank 0 */
#define ICM20649_BANK_1                  	((uint8_t)1 << 7)     /**< Register bank 1 */
#define ICM20649_BANK_2                  	((uint8_t)2 << 7)     /**< Register bank 2 */
#define ICM20649_BANK_3                  	((uint8_t)3 << 7)     /**< Register bank 3 */

// Bank 0
#define ICM20X_B0_WHOAMI 0x00         ///< Chip ID register
#define ICM20X_B0_USER_CTRL 0x03      ///< User Control Reg. Includes I2C Master
#define ICM20X_B0_LP_CONFIG 0x05      ///< Low Power config
#define ICM20X_B0_REG_INT_PIN_CFG 0xF ///< Interrupt config register
#define ICM20X_B0_REG_INT_ENABLE 0x10 ///< Interrupt enable register 0
#define ICM20X_B0_REG_INT_ENABLE_1 0x11 ///< Interrupt enable register 1
#define ICM20X_B0_I2C_MST_STATUS                                               \
  0x17 ///< Records if I2C master bus data is finished
#define ICM20X_B0_REG_BANK_SEL 0x7F ///< register bank selection register
#define ICM20X_B0_PWR_MGMT_1 0x06   ///< primary power management register
#define ICM20X_B0_PWR_MGMT_2 0x07   ///< secondary power management register
#define ICM20X_B0_ACCEL_XOUT_H 0x2D ///< first byte of accel data
#define ICM20X_B0_GYRO_XOUT_H 0x33  ///< first byte of gyro data
#define ICM20X_B0_GYRO_XOUT_L 0x34  ///< second byte of gyro data
#define ICM20X_B0_TEMP_OUT_H  0x39  ///< first byte of temp data
#define ICM20X_B0_TEMP_OUT_L  0x40  ///< first byte of temp data


// Bank 2
#define ICM20X_B2_GYRO_SMPLRT_DIV 0x00    ///< Gyroscope data rate divisor
#define ICM20X_B2_GYRO_CONFIG_1 0x01      ///< Gyro config for range setting
#define ICM20X_B2_ACCEL_SMPLRT_DIV_1 0x10 ///< Accel data rate divisor MSByte
#define ICM20X_B2_ACCEL_SMPLRT_DIV_2 0x11 ///< Accel data rate divisor LSByte
#define ICM20X_B2_ACCEL_CONFIG_1 0x14     ///< Accel config for setting range

// Bank 3
#define ICM20X_B3_I2C_MST_ODR_CONFIG 0x0 ///< Sets ODR for I2C master bus
#define ICM20X_B3_I2C_MST_CTRL 0x1       ///< I2C master bus config
#define ICM20X_B3_I2C_MST_DELAY_CTRL 0x2 ///< I2C master bus config
#define ICM20X_B3_I2C_SLV0_ADDR                                                \
  0x3 ///< Sets I2C address for I2C master bus slave 0
#define ICM20X_B3_I2C_SLV0_REG                                                 \
  0x4 ///< Sets register address for I2C master bus slave 0
#define ICM20X_B3_I2C_SLV0_CTRL 0x5 ///< Controls for I2C master bus slave 0
#define ICM20X_B3_I2C_SLV0_DO 0x6   ///< Sets I2C master bus slave 0 data out

#define ICM20X_B3_I2C_SLV4_ADDR                                                \
  0x13 ///< Sets I2C address for I2C master bus slave 4
#define ICM20X_B3_I2C_SLV4_REG                                                 \
  0x14 ///< Sets register address for I2C master bus slave 4
#define ICM20X_B3_I2C_SLV4_CTRL 0x15 ///< Controls for I2C master bus slave 4
#define ICM20X_B3_I2C_SLV4_DO 0x16   ///< Sets I2C master bus slave 4 data out
#define ICM20X_B3_I2C_SLV4_DI 0x17   ///< Sets I2C master bus slave 4 data in

#define ICM20948_CHIP_ID 0xEA ///< ICM20948 default device id from WHOAMI
#define ICM20649_CHIP_ID 0xE1 ///< ICM20649 default device id from WHOAMI

/** Options for `enableAccelDLPF` */
typedef enum {
  ICM20X_ACCEL_FREQ_246_0_HZ = 0x1,
  ICM20X_ACCEL_FREQ_111_4_HZ = 0x2,
  ICM20X_ACCEL_FREQ_50_4_HZ = 0x3,
  ICM20X_ACCEL_FREQ_23_9_HZ = 0x4,
  ICM20X_ACCEL_FREQ_11_5_HZ = 0x5,
  ICM20X_ACCEL_FREQ_5_7_HZ = 0x6,
  ICM20X_ACCEL_FREQ_473_HZ = 0x7,
} icm20x_accel_cutoff_t;

/** Options for `enableGyroDLPF` */
typedef enum {
  ICM20X_GYRO_FREQ_196_6_HZ = 0x0,
  ICM20X_GYRO_FREQ_151_8_HZ = 0x1,
  ICM20X_GYRO_FREQ_119_5_HZ = 0x2,
  ICM20X_GYRO_FREQ_51_2_HZ = 0x3,
  ICM20X_GYRO_FREQ_23_9_HZ = 0x4,
  ICM20X_GYRO_FREQ_11_6_HZ = 0x5,
  ICM20X_GYRO_FREQ_5_7_HZ = 0x6,
  ICM20X_GYRO_FREQ_361_4_HZ = 0x7,

} icm20x_gyro_cutoff_t;


#endif
