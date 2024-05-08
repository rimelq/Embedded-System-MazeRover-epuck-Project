#ifndef MOTOR_MODULE_H
#define MOTOR_MODULE_H


// Variables for navigation IR/MOTORS
#define SPEED_INCREMENT 35  // 35
#define MOTOR_SPEED_LIMIT 1100
#define MOTOR_CRUISING_SPEED 250  // 250
#define DETLA_LIMIT 70  // 70
#define TURNING_TIME_90DEG 1230  // 1230

// Variables for starting the IMU thread function
#define DISABLE_IMU false
#define ENABLE_IMU true

// motor controller function declaration
void motor_controller_start(void);
void motor_controller_turn_90_deg (uint8_t turn_direction);
void motors_speed_update(int16_t right, int16_t left);
int16_t motor_speed_increment(int16_t current_speed, int16_t other_motor_speed);
bool get_start_imu(void);

#endif /* MOTOR_MODULE_H */
