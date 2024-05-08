#ifndef IMU_MODULE_H
#define IMU_MODULE_H

#include <sensors/imu.h>  // include proximity module for all needed modules

// Variable for tilting direction IMU
#define LEFT_TILT 0
#define RIGHT_TILT 1
#define NO_TILT 2

// IMU function declaration
void imu_module_start(void);
uint8_t show_gravity(imu_msg_t *imu_values);
uint8_t get_orientation_motor(void);

#endif