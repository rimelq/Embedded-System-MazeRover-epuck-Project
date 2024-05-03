#ifndef IR_MODULE_H
#define IR_MODULE_H

#define FRONT_SENSOR_CHANNEL 0  // Example channel number for front sensor
#define RIGHT_SENSOR_CHANNEL 2  // Example channel number for right sensor
#define LEFT_SENSOR_CHANNEL 5   // Example channel number for left sensor

void ir_module_init(void);
void ir_module_start(void);

#endif /* IR_MODULE_H */