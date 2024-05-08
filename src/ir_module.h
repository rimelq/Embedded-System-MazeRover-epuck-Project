#ifndef IR_MODULE_H
#define IR_MODULE_H

#include <sensors/proximity.h>

#define FRONT_SENSOR_CHANNEL 0  // Channel number for front sensor
#define RIGHT_SENSOR_CHANNEL 2  // Channel number for right sensor
#define LEFT_SENSOR_CHANNEL 5   // Channel number for left sensor

// Variables for motor controller
#define STOP_MOTOR 0
#define RIGHT_MOTOR 1
#define LEFT_MOTOR 2
#define CRUISE 3

// Variables for IR processing
#define OBSTACLE_DISTANCE 400  // 600 ou 400 (works better for now)
#define CRUISING_THRESHOLD_RIGHT 800  // 800
#define CRUISING_THRESHOLD_LEFT 600  // 600

// Functions declaration
void ir_module_start(void);
uint8_t get_ir_message(void);

#endif /* IR_MODULE_H */