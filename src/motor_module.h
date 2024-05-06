#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H

// Define some variables
#define RIGHT_CORRECTION 0
#define LEFT_CORRECTION 1
#define STOP_MOTOR 2
#define RIGHT_TURN 0
#define LEFT_TURN 1

#define SPEED_INCREMENT 70
#define MOTOR_SPEED_LIMIT 1100
#define MOTOR_CRUISING_SPEED 500
#define DETLA_LIMIT 200
#define TURNING_TIME_90DEG 600


// motor controller functions
void motor_controller_init(void);
void motor_controller_start(void);
void motor_controller_turn_90_deg (uint16_t turn_direction);
void motors_speed_update(int16_t right, int16_t left);
int16_t motor_speed_increment(int16_t current_speed, int16_t other_motor_speed);

#endif /* MOTOR_MODULE_H */
