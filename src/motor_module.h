#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

// start the motor controller thread
void motor_controller_start(void);

// motor controller functions
void motor_controller_init(void);
void motor_controller_turn_90_deg (int16_t turn_direction);

#endif /* MOTOR_MODULE_H */
