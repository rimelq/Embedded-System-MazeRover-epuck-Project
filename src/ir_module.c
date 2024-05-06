#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include <sensors/proximity.h>
#include <stdlib.h>

#include "ir_module.h"

// Initialize message bus topics
extern messagebus_t bus;
static uint8_t ir_message;


// IR module thread: read and send IR data constantly on topic message bus
static THD_WORKING_AREA(waIRSensorThread, 256);
static THD_FUNCTION(IRSensorThread, arg) {
    (void)arg;
    chRegSetThreadName("IRSensorReading");

    
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
        if (front_value > OBSTACLE_DISTANCE) {
            led1 = 1;
            ir_message = STOP_MOTOR;  // stop both motors
        }
        else if (abs(right_value - left_value) >= CRUISING_DIFFERENCE_THRESHOLD) {  // case: too much difference
            if (right_value > left_value) {
                led3 = 1;
                ir_message = RIGHT_MOTOR;  // increase right motor speed
            }
            else if (right_value < left_value) {
                led7 = 1;
                ir_message = LEFT_MOTOR;  // increase left motor speed
            }
        }
        else {
            ir_message = CRUISE;  // increase left motor speed
        }
        chSysUnlock();
        
        //we invert the values because a led is turned on if the signal is low
        palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
        palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
        palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);

        chThdSleepMilliseconds(100);  // sleep to give hand to other module
    }
}


// Function to start IR module
void ir_module_init(void) {
    // NOTHING ?
}

// Function that starts the thread
void ir_module_start(void) {
    chThdCreateStatic(waIRSensorThread, sizeof(waIRSensorThread), NORMALPRIO+1, IRSensorThread, NULL);
}

// Function that sends the ir command to motor controller module
uint8_t get_ir_message(void) {
    return ir_message;
}
