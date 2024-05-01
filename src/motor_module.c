#include <ch.h>
#include <hal.h>
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>

#include "motor_module.h"

// Define some variables
#define RIGHT 0
#define LEFT 1

// Thread for the motor controller module
static THD_WORKING_AREA(waMotorsModule, 256);
static THD_FUNCTION(MotorsModule, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    int16_t speed_right = 0;
    int16_t speed_left = 0;

    int16_t turn_direction = 0;

    motor_controller_turn_90_deg(RIGHT);

    while(1){
        time = chVTGetSystemTime();

        right_motor_set_speed(speed_right);
        left_motor_set_speed(speed_left);

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

// Function that starts the motor controller thread from Main
void motor_controller_start(void) {
	chThdCreateStatic(waMotorsModule, sizeof(waMotorsModule), NORMALPRIO, MotorsModule, NULL);
}

// Function that initialized the motor controller module
void motor_controller_init(void) {

    motors_init();
    motor_controller_start();
}

// Function that turns the robot 90 degrees in a certain direction (left/right)
void motor_controller_turn_90_deg (int16_t turn_direction) {

    int32_t actual_right_position = right_motor_get_pos();
    int32_t actual_left_position = left_motor_get_pos();
    int32_t target_right_position = 0;
    int32_t target_left_position = 0;
    right_motor_set_speed(500);
    left_motor_set_speed(500);

    if (turn_direction == RIGHT) {
        target_right_position = actual_right_position - 500;
        target_left_position = actual_left_position + 500;
    }
    else if (turn_direction == LEFT) {
        target_right_position = actual_right_position + 500;
        target_left_position = actual_left_position - 500;
    }
    right_motor_set_pos(target_right_position);
    left_motor_set_pos(target_left_position);
}