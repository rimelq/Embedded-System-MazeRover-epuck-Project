#ifndef IR_MODULE_H
#define IR_MODULE_H

#define FRONT_SENSOR_CHANNEL 0  // Example channel number for front sensor
#define RIGHT_SENSOR_CHANNEL 2  // Example channel number for right sensor
#define LEFT_SENSOR_CHANNEL 5   // Example channel number for left sensor

// Variables for motor controller
#define STOP_MOTOR 0
#define RIGHT_MOTOR 1
#define LEFT_MOTOR 2
#define CRUISE 3

// Variables for IR processing
#define OBSTACLE_DISTANCE 450
#define CRUISING_DIFFERENCE_THRESHOLD 30

// Functions
void ir_module_init(void);
void ir_module_start(void);
uint8_t get_ir_message(void);

#endif /* IR_MODULE_H */