#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include <sensors/proximity.h>
#include <stdlib.h>
#include "ir_module.h"

// Initialize message bus topics
extern messagebus_t bus;
// messagebus_topic_t ir_front_obstacle_topic, ir_right_obstacle_topic, ir_left_obstacle_topic;
// static MUTEX_DECL(ir_front_obstacle_topic_lock);
// static MUTEX_DECL(ir_right_obstacle_topic_lock);
// static MUTEX_DECL(ir_left_obstacle_topic_lock);
// static CONDVAR_DECL(ir_front_obstacle_topic_condvar);
// static CONDVAR_DECL(ir_right_obstacle_topic_condvar);
// static CONDVAR_DECL(ir_left_obstacle_topic_condvar);
static uint8_t ir_message;


// Declares the topic on the bus.
// messagebus_topic_t ir_to_motor_topic;
// MUTEX_DECL(ir_to_motor_topic_lock);
// CONDVAR_DECL(ir_to_motor_topic_condvar);


// IR module thread: read and send IR data constantly on topic message bus
static THD_WORKING_AREA(waIRSensorThread, 256);
static THD_FUNCTION(IRSensorThread, arg) {
    (void)arg;
    chRegSetThreadName("IRSensorReading");

    
    uint16_t front_value, right_value, left_value;
    messagebus_topic_t *proximity_topic = messagebus_find_topic_blocking(&bus, "/proximity");
    proximity_msg_t prox_values;

    // messagebus_topic_init(&ir_to_motor_topic, &ir_to_motor_topic_lock, &ir_to_motor_topic_condvar, &ir_message, sizeof(ir_message));
    // messagebus_advertise_topic(&bus, &ir_to_motor_topic, "/irTOmotor");
    

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
            // messagebus_topic_publish(&ir_to_motor_topic, &ir_message, sizeof(ir_message));
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
            // messagebus_topic_publish(&ir_to_motor_topic, &ir_message, sizeof(ir_message));
        }
        chSysUnlock();

        // Send the IR values on topic message bus
        // messagebus_topic_publish(&ir_front_obstacle_topic, &front_value, sizeof(front_value));
        // messagebus_topic_publish(&ir_right_obstacle_topic, &right_value, sizeof(right_value));
        // messagebus_topic_publish(&ir_left_obstacle_topic, &left_value, sizeof(left_value));
        
        //chThdYield();  // Gives hand to next thread in Round Robin (-> Navigation Thread)
        //we invert the values because a led is turned on if the signal is low
        palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
        palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);
        palWritePad(GPIOD, GPIOD_LED1, led1 ? 0 : 1);
        chThdSleepMilliseconds(100);
    }
}


// Function to start IR module
void ir_module_init(void) {

    // messagebus_topic_init(&ir_front_obstacle_topic, &ir_front_obstacle_topic_lock, &ir_front_obstacle_topic_condvar, NULL, 0);
    // messagebus_advertise_topic(&bus, &ir_front_obstacle_topic, "/ir_front_obstacle");

    // messagebus_topic_init(&ir_right_obstacle_topic, &ir_right_obstacle_topic_lock, &ir_right_obstacle_topic_condvar, NULL, 0);
    // messagebus_advertise_topic(&bus, &ir_right_obstacle_topic, "/ir_right_obstacle");

    // messagebus_topic_init(&ir_left_obstacle_topic, &ir_left_obstacle_topic_lock, &ir_left_obstacle_topic_condvar, NULL, 0);
    // messagebus_advertise_topic(&bus, &ir_left_obstacle_topic, "/ir_left_obstacle");

}

// Function that starts the thread
void ir_module_start(void) {
    //calibrate_ir();
    //proximity_start();
    chThdCreateStatic(waIRSensorThread, sizeof(waIRSensorThread), NORMALPRIO+1, IRSensorThread, NULL);
}

uint8_t get_ir_message(void) {
    return ir_message;
}
