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
#define OBSTACLE_DISTANCE 400
#define CRUISING_THRESHOLD_RIGHT 550
#define CRUISING_THRESHOLD_LEFT 400
// IR variables to detect intersection to stop motor correction
#define MIN_THRESHOLD_RIGHT 200
#define MIN_THRESHOLD_LEFT 200
#define MAX_THRESHOLD_RIGHT 2000
#define MAX_THRESHOLD_LEFT 1800
// IR variable to control max time (number of loop) for motor correction left/right
#define MAX_CORRECTION_NBR 6  // to prevent long correction angles

// Functions declaration
void ir_module_start(void);
uint8_t get_ir_message(void);
void navigation_robot(uint16_t front, uint16_t right, uint16_t left);

#endif /* IR_MODULE_H */