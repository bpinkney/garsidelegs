/*
 * garsidelegs_hw_sensors.h
 */

#ifndef INC_GARSIDELEGS_HW_SENSORS_H_
#define INC_GARSIDELEGS_HW_SENSORS_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "Adafruit_ICM20649.h"
#include "Adafruit_ICM20X.h"


// functions
uint8_t garsidelegs_hw_sensors_imu_init(void);
void garsidelegs_hw_sensors_imu_process_interrupt_sample(void);
void garsidelegs_hw_sensors_imu_process_poll_sample(void);

// move these natural constants later
#define GRAV (9.80665) // m/s^2 for 1 g

#endif /* INC_GARSIDELEGS_HW_SENSORS_H_ */
