#include <ch.h>
#include <hal.h>
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>
#include <msgbus/messagebus.h>

#include "motor_module.h"


// Initialize message bus topics
extern messagebus_t bus;

// Thread for the motor controller module
static THD_WORKING_AREA(waMotorsModule, 256);
static THD_FUNCTION(MotorsModule, arg) {

    chRegSetThreadName("MotorController");
    (void)arg;

    // Find message bus topics
    messagebus_topic_t *direction_topic = messagebus_find_topic_blocking(&bus, "/motor_direction_imu");
    messagebus_topic_t *correction_topic = messagebus_find_topic_blocking(&bus, "/motor_correction_ir");

    uint16_t motor_navigation_correction, motor_turn_direction;
    int16_t speed_right = 0;
    int16_t speed_left = 0;


    while(1){

        // Wait for navigation module command
        messagebus_topic_wait(correction_topic, &motor_navigation_correction, sizeof(motor_navigation_correction));

        // Put the robot into cruising (normal) mode forward
        speed_right = MOTOR_CRUISING_SPEED;
        speed_left = MOTOR_CRUISING_SPEED;
        motors_speed_update(speed_right, speed_left);

        switch(motor_navigation_correction) {
            case STOP_MOTOR:
                speed_right = 0;
                speed_left = 0;
                motors_speed_update(speed_right, speed_left);
                // Wait for navigation turn direction command coming from IMU data
                messagebus_topic_wait(direction_topic, &motor_turn_direction, sizeof(motor_turn_direction));
                if (motor_turn_direction == RIGHT_TURN) {
                    motor_controller_turn_90_deg(RIGHT_TURN);  // turn 90deg to the right
                }
                else if (motor_turn_direction == LEFT_TURN) {
                    motor_controller_turn_90_deg(LEFT_TURN);  // turn 90deg to the left
                }
                break;
            case RIGHT_CORRECTION:
                speed_right = motor_speed_increment(speed_right);
                motors_speed_update(speed_right, speed_left);
                break;
            case LEFT_CORRECTION:
                speed_left = motor_speed_increment(speed_left);
                motors_speed_update(speed_right, speed_left);
                break;
            default:
                break;
        }
        chThdSleepMilliseconds(100);  // Sleep thread
    }
}

// Function that initialized the motor controller module
void motor_controller_init(void) {
    motors_init();
    chThdCreateStatic(waMotorsModule, sizeof(waMotorsModule), NORMALPRIO, MotorsModule, NULL);
}

// Function that updates the speed of each motor
void motors_speed_update(int16_t right, int16_t left) {
    right_motor_set_speed(right);
    left_motor_set_speed(left);
}

// Function that turns the robot 90 degrees in a certain direction (left/right)
void motor_controller_turn_90_deg (uint16_t turn_direction) {
    if (turn_direction == RIGHT_TURN) {
        right_motor_set_speed(-500);
        left_motor_set_speed(500);
    }
    else if (turn_direction == LEFT_TURN) {
        right_motor_set_speed(500);
        left_motor_set_speed(-500);
    }
    chThdSleepMilliseconds(TURNING_TIME_90DEG);  // sleep duration to make the 90deg turn
    // Stop the motors
    right_motor_set_speed(0);
    left_motor_set_speed(0);
    chThdSleepMilliseconds(TURNING_TIME_90DEG);
    right_motor_set_speed(MOTOR_CRUISING_SPEED);
    left_motor_set_speed(MOTOR_CRUISING_SPEED);
}

// Function that increments the motor speed for movement correction
int16_t motor_speed_increment(int16_t current_speed) {
    int16_t new_speed = current_speed + SPEED_INCREMENT;
    if (new_speed > MOTOR_SPEED_LIMIT) {  // never the case but just for protection
        new_speed = MOTOR_SPEED_LIMIT;
    }
    return new_speed;
}