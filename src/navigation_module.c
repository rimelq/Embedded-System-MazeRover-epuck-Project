#include "ch.h"
#include "hal.h"
#include <stdlib.h>
#include <msgbus/messagebus.h>
#include "navigation_module.h"


// Initialize message bus topics
extern messagebus_t bus;
messagebus_topic_t motor_direction_imu, motor_correction_ir, imu_wake_up;
static MUTEX_DECL(motor_direction_imu_lock);
static MUTEX_DECL(motor_correction_ir_lock);
static MUTEX_DECL(imu_wake_up_lock);
static CONDVAR_DECL(motor_direction_imu_condvar);
static CONDVAR_DECL(motor_correction_ir_condvar);
static CONDVAR_DECL(imu_wake_up_condvar);


// Navigation module thread: receive IR data, control motors, receive IMU data
static THD_WORKING_AREA(waNavigationThread, 512);
static THD_FUNCTION(NavigationThread, arg) {
    (void)arg;
    chRegSetThreadName("Navigation");
    
    uint16_t front_obstacle_distance, right_obstacle_distance, left_obstacle_distance, turn_direction_imu, stop_motor_ir;
    bool ir_alert_distance = false;
    bool imu_wake_up_signal = true;

    // Find message bus topics
    messagebus_topic_t *front_topic = messagebus_find_topic_blocking(&bus, "/ir_front_obstacle");
    messagebus_topic_t *right_topic = messagebus_find_topic_blocking(&bus, "/ir_right_obstacle");
    messagebus_topic_t *left_topic = messagebus_find_topic_blocking(&bus, "/ir_left_obstacle");
    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu_orientation");

    while (true) {

        // STAY ALERT TO GET TOPIC MESSAGES FROM IR
        // wait for Left topic since its data arrives last, then read the Right and Front data
        messagebus_topic_wait(left_topic, &left_obstacle_distance, sizeof(left_obstacle_distance));
        messagebus_topic_read(front_topic, &front_obstacle_distance, sizeof(front_obstacle_distance));
        messagebus_topic_read(right_topic, &right_obstacle_distance, sizeof(right_obstacle_distance));

        ir_alert_distance = ir_process_data(front_obstacle_distance, right_obstacle_distance, left_obstacle_distance);
        if (ir_alert_distance) {  // case: front obstacle detected
            // STOP MOTORS: send to motor controller module the stop command
            stop_motor_ir = STOP_MOTOR;
            messagebus_topic_publish(&motor_correction_ir, &stop_motor_ir, sizeof(stop_motor_ir));
            // START: IMU FUNCTION
            messagebus_topic_publish(&imu_wake_up, &imu_wake_up_signal, sizeof(imu_wake_up_signal));
            messagebus_topic_wait(imu_topic, &turn_direction_imu, sizeof(turn_direction_imu));
            // SEND: motor module the turn direction
            messagebus_topic_publish(&motor_direction_imu, &turn_direction_imu, sizeof(turn_direction_imu));
        }
        chThdSleepMilliseconds(100);  // Sleep thread
    }
}

// Function to initialise thread and created topics
void navigation_module_init(void) {

    messagebus_topic_init(&motor_direction_imu, &motor_direction_imu_lock, &motor_direction_imu_condvar, NULL, 0);
    messagebus_advertise_topic(&bus, &motor_direction_imu, "/motor_direction_imu");

    messagebus_topic_init(&motor_correction_ir, &motor_correction_ir_lock, &motor_correction_ir_condvar, NULL, 0);
    messagebus_advertise_topic(&bus, &motor_correction_ir, "/motor_correction_ir");

    messagebus_topic_init(&imu_wake_up, &imu_wake_up_lock, &imu_wake_up_condvar, NULL, 0);
    messagebus_advertise_topic(&bus, &imu_wake_up, "/imu_wake_up");

}

// Function that starts the thread
void navigation_module_start(void) {
    chThdCreateStatic(waNavigationThread, sizeof(waNavigationThread), NORMALPRIO, NavigationThread, NULL);
}

// Function to analyse and process IR values
bool ir_process_data(uint16_t front, uint16_t right, uint16_t left) {

    uint8_t motor_correction;

    // Front IR data
    if (front >= OBSTACLE_THRESHOLD_IR) {  // case: front obstacle
        return true;
    }

    // Right/Left IR data for motor correction
    if (abs(right - left) > DIFFERENCE_THRESHOLD_IR) {
        if (right > left) {  // case: too close to the right
            // SEND: increase right motor speed with regulator
            motor_correction = RIGHT_CORRECTION;
            messagebus_topic_publish(&motor_correction_ir, &motor_correction, sizeof(motor_correction));
        }
        else if (right < left) {  // case: too close to the left
            // SEND: increase left motor speed with regulator
            motor_correction = LEFT_CORRECTION;
            messagebus_topic_publish(&motor_correction_ir, &motor_correction, sizeof(motor_correction));
        }
    }
    return false;
}
