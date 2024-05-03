#ifndef IMU_MODULE_H
#define IMU_MODULE_H

#include <sensors/imu.h>

// Useful variables
#define RIGHT_TURN 0
#define LEFT_TURN 1
#define GRAVITY_THRESHOLD 0.2f  //threshold value regulate horizontal variation noise

// functions definitions
void imu_module_init(void);
void imu_module_start(void);
bool show_gravity(imu_msg_t *imu_values);

#endif