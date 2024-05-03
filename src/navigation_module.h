#ifndef NAVIGATION_MODULE_H
#define NAVIGATION_MODULE_H

// define useful variables
#define OBSTACLE_THRESHOLD_IR 200
#define DIFFERENCE_THRESHOLD_IR 100
#define RIGHT_CORRECTION 0
#define LEFT_CORRECTION 1
#define RIGHT_TURN 0
#define LEFT_TURN 1
#define STOP_MOTOR 2

void navigation_module_init(void);
void navigation_module_start(void);
bool ir_process_data(uint16_t front, uint16_t right, uint16_t left);

#endif // NAVIGATION_MODULE_H