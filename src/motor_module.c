#include <ch.h>
#include <hal.h>
#include <motors.h>

#include "motor_module.h"
#include "ir_module.h"
#include "imu_module.h"


// Initialize static variables
static bool start_imu = DISABLE_IMU;
static uint8_t imu_orientation = NO_TILT;


// Thread for the motor controller module
static THD_WORKING_AREA(waMotorsModule, 256);
static THD_FUNCTION(MotorsModule, arg) {
    (void)arg; chRegSetThreadName("MotorController");
    static uint8_t ir_command;  // create static variables for the thread
    static int16_t speed_right = 0, speed_left = 0;
    while(true){
        if (start_imu == DISABLE_IMU) { // case: IMU not needed (normal cruising mode)
            ir_command = get_ir_message();  // get command to execute from IR thread
            switch (ir_command) {
                case STOP_MOTOR:  // case: stop motors and wait for IMU directives
                    speed_right = 0; speed_left = 0;
                    motors_speed_update(speed_right, speed_left);
                    start_imu = ENABLE_IMU;
                    break;
                case RIGHT_MOTOR:  // case: right motor correction
                    speed_left = MOTOR_CRUISING_SPEED;
                    speed_right = motor_speed_increment(speed_right, speed_left);
                    motors_speed_update(speed_right, speed_left);
                    break;
                case LEFT_MOTOR:  // case: left motor correction
                    speed_right = MOTOR_CRUISING_SPEED;
                    speed_left = motor_speed_increment(speed_left, speed_right);
                    motors_speed_update(speed_right, speed_left);
                    break;
                case CRUISE:  // case: normal cruising mode
                    speed_right = MOTOR_CRUISING_SPEED;
                    speed_left = MOTOR_CRUISING_SPEED;
                    motors_speed_update(speed_right, speed_left);
                    break;  
                default: break; }
        }
        else if (start_imu == ENABLE_IMU) {  // case: waiting for IMU thread response
            imu_orientation = get_orientation_motor();  // get turn direction from IMU
            if ((imu_orientation == RIGHT_TILT) || (imu_orientation == LEFT_TILT)) {
                start_imu = DISABLE_IMU;
                motor_controller_turn_90_deg(imu_orientation);  // turn 90deg
                speed_right = MOTOR_CRUISING_SPEED;  // reset the cruising speed right
                speed_left = MOTOR_CRUISING_SPEED;  // reset the cruising speed left
            }
            imu_orientation = NO_TILT;  // reset the static variable for future event
        }
        chThdSleepMilliseconds(50);  // sleep to give hand to other module
    }
}


// Function that starts the thread
void motor_controller_start(void) {
    chThdCreateStatic(waMotorsModule, sizeof(waMotorsModule), NORMALPRIO+1, 
    MotorsModule, NULL);
}

// Function that updates the speed of each motor
void motors_speed_update(int16_t right, int16_t left) {
    right_motor_set_speed(right);
    left_motor_set_speed(left);
}

// Function that turns the robot 90 degrees in a certain direction (left/right)
void motor_controller_turn_90_deg (uint8_t turn_direction) {
    if (turn_direction == RIGHT_TILT) {
        right_motor_set_speed(-MOTOR_CRUISING_SPEED);
        left_motor_set_speed(MOTOR_CRUISING_SPEED);
    }
    else if (turn_direction == LEFT_TILT) {
        right_motor_set_speed(MOTOR_CRUISING_SPEED);
        left_motor_set_speed(-MOTOR_CRUISING_SPEED);
    }
    chThdSleepMilliseconds(TURNING_TIME_90DEG);  // sleep duration for 90deg turn
    // Stop the motors
    right_motor_set_speed(0);
    left_motor_set_speed(0);
}

// Function that increments the motor speed for movement correction
int16_t motor_speed_increment(int16_t current_speed, int16_t other_motor_speed) {
    int16_t new_speed = current_speed + SPEED_INCREMENT;
    int16_t delta = new_speed - other_motor_speed;
    if (delta > DELTA_LIMIT) {  // to avoid too much difference in speed
        new_speed = current_speed;
    }
    if (new_speed > MOTOR_SPEED_LIMIT) {  // never the case but just for protection
        new_speed = MOTOR_SPEED_LIMIT;
    }
    return new_speed;
}

// Function that sends the imu wake up
bool get_start_imu(void) {
    return start_imu;
}