#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include <stdlib.h>

#include "ir_module.h"

// Initialize message bus topics and static variables
extern messagebus_t bus;
static uint8_t ir_message = CRUISE;
static uint8_t correction_loop_number = 0;


// IR module thread: read IR proximity measures and give command to motor thread
static THD_WORKING_AREA(waIRSensorThread, 256);
static THD_FUNCTION(IRSensorThread, arg) {
    (void)arg;
    chRegSetThreadName("IRSensorReading");
    // variables and messagebus declaration
    uint16_t front_value, right_value, left_value;
    messagebus_topic_t *proximity_topic = messagebus_find_topic_blocking(&bus, 
    "/proximity");
    proximity_msg_t prox_values;

    while (true) {

        // wait for the proximity topic to send measures
        messagebus_topic_wait(proximity_topic, &prox_values, sizeof(prox_values));

        // Fetch data using the proximity API
        front_value = get_prox(FRONT_SENSOR_CHANNEL);
        right_value = get_prox(RIGHT_SENSOR_CHANNEL);
        left_value = get_prox(LEFT_SENSOR_CHANNEL);

        chSysLock();
        navigation_robot(front_value, right_value, left_value);
        chSysUnlock();

        chThdSleepMilliseconds(50);  // sleep to give hand to other module
    }
}


// Function that starts the thread
void ir_module_start(void) {
    chThdCreateStatic(waIRSensorThread, sizeof(waIRSensorThread), NORMALPRIO+1, 
    IRSensorThread, NULL);
}

// Function that sends the ir command to motor controller module
uint8_t get_ir_message(void) {
    return ir_message;
}

// Function that does the navigation logic
void navigation_robot(uint16_t front, uint16_t right, uint16_t left) {
    uint8_t led3 = 0, led7 = 0, led1 = 0;

    if (front > OBSTACLE_DISTANCE) {  // case: detected front obstacle
            led1 = 1;
            ir_message = STOP_MOTOR;  // stop both motors
            correction_loop_number = 0; // reset correction loop number for next event
        }
    else if (((right >= CRUISING_THRESHOLD_RIGHT) && 
                (left >= MIN_THRESHOLD_LEFT) && 
                (correction_loop_number <= MAX_CORRECTION_NBR) && 
                (right > left)) || 
                (right >= MAX_THRESHOLD_RIGHT)) {  // case: closer to Right
        led3 = 1;
        ir_message = RIGHT_MOTOR;  // increase right motor speed
        correction_loop_number += 1;  // increment the loop number
    }
    else if (((left >= CRUISING_THRESHOLD_LEFT) && 
                (right >= MIN_THRESHOLD_RIGHT) && 
                (correction_loop_number <= MAX_CORRECTION_NBR) && 
                (right < left)) || 
                (left >= MAX_THRESHOLD_LEFT)) {  // case: closer to Left
        led7 = 1;
        ir_message = LEFT_MOTOR;  // increase left motor speed
        correction_loop_number += 1;  // increment the loop number
    }
    else {
        ir_message = CRUISE;  // put robot in normal cruising mode
        correction_loop_number = 0; // reset correction loop number for next event
    }
    // Turn ON Leds
    palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);  
    palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
    palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
}