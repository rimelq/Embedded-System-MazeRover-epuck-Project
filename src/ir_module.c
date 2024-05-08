#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include <stdlib.h>

#include "ir_module.h"

// Initialize message bus topics and static variables
extern messagebus_t bus;
static uint8_t ir_message = CRUISE;


// IR module thread: read IR proximity measures and give command to motor thread
static THD_WORKING_AREA(waIRSensorThread, 256);
static THD_FUNCTION(IRSensorThread, arg) {
    (void)arg;
    chRegSetThreadName("IRSensorReading");

    // variables and messagebus declaration
    uint16_t front_value, right_value, left_value;
    messagebus_topic_t *proximity_topic = messagebus_find_topic_blocking(&bus, "/proximity");
    proximity_msg_t prox_values;
    

    while (true) {
        uint8_t led3 = 0, led7 = 0, led1 = 0;
        // wait for the proximity topic to send measures
        messagebus_topic_wait(proximity_topic, &prox_values, sizeof(prox_values));
        // Fetch data using the proximity API
        front_value = get_prox(FRONT_SENSOR_CHANNEL);
        right_value = get_prox(RIGHT_SENSOR_CHANNEL);
        left_value = get_prox(LEFT_SENSOR_CHANNEL);

        chSysLock();
        if (front_value > OBSTACLE_DISTANCE) {  // case: detected an obstacle on the front
            led1 = 1;
            ir_message = STOP_MOTOR;  // stop both motors
        }
        // else if ((abs(right_value - left_value) >= CRUISING_DIFFERENCE_THRESHOLD) && (abs(right_value - left_value) < CRUISING_DIFFERENCE_MAX)) {  // case: too much difference
        //     if (right_value > left_value) {  // case: closer to the Right
        //         led3 = 1;
        //         ir_message = RIGHT_MOTOR;  // increase right motor speed
        //     }
        //     else if (right_value < left_value) {  // case: closer to the Left
        //         led7 = 1;
        //         ir_message = LEFT_MOTOR;  // increase left motor speed
        //     }
        // }
        else if (right_value >= CRUISING_THRESHOLD_RIGHT) {  // case: closer to the Right
            led3 = 1;
            ir_message = RIGHT_MOTOR;  // increase right motor speed
        }
        else if (left_value >= CRUISING_THRESHOLD_LEFT) {  // case: closer to the Left
            led7 = 1;
            ir_message = LEFT_MOTOR;  // increase left motor speed
        }
        else {
            ir_message = CRUISE;  // put robot in normal cruising mode
        }
        chSysUnlock();
        
        // Turn ON Leds: we invert the values because a led is turned on if the signal is low
        palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
        palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
        palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);

        chThdSleepMilliseconds(50);  // sleep to give hand to other module
    }
}


// Function that starts the thread
void ir_module_start(void) {
    chThdCreateStatic(waIRSensorThread, sizeof(waIRSensorThread), NORMALPRIO+1, IRSensorThread, NULL);
}

// Function that sends the ir command to motor controller module
uint8_t get_ir_message(void) {
    return ir_message;
}