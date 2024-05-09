#ifndef IR_MODULE_H
#define IR_MODULE_H

#include <sensors/proximity.h>  // include proximity module for all needed modules

#define FRONT_SENSOR_CHANNEL 0  // Channel number for front sensor
#define RIGHT_SENSOR_CHANNEL 2  // Channel number for right sensor
#define LEFT_SENSOR_CHANNEL 5   // Channel number for left sensor

// Variables for motor controller
#define STOP_MOTOR 0
#define RIGHT_MOTOR 1
#define LEFT_MOTOR 2
#define CRUISE 3

// Variables for IR processing
#define OBSTACLE_DISTANCE 450  // 600 or 400 (works better for now) or 450 (works better+1)
#define CRUISING_THRESHOLD_RIGHT 700  // 800 or 700 (works better for now)
#define CRUISING_THRESHOLD_LEFT 600  // 600
// IR variables to detect intersection to stop motor correction
#define MIN_THRESHOLD_RIGHT 120  // 100 (works good) or 120 (works better)
#define MIN_THRESHOLD_LEFT 100  // 100 (works good)
// IR variable to control max amount of time (number of loop) permitted for motor correction left/right
#define MAX_CORRECTION_NBR 5  // to prevent long correction angles

// Functions declaration
void ir_module_start(void);
uint8_t get_ir_message(void);

#endif /* IR_MODULE_H */