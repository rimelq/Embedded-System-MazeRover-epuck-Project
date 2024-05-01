#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include <sensors/proximity.h>
#include "ir_module.h"

extern messagebus_t bus;
messagebus_topic_t ir_front_obstacle_topic, ir_right_obstacle_topic, ir_left_obstacle_topic;
static MUTEX_DECL(ir_front_obstacle_topic_lock);
static MUTEX_DECL(ir_right_obstacle_topic_lock);
static MUTEX_DECL(ir_left_obstacle_topic_lock);
static CONDVAR_DECL(ir_front_obstacle_topic_condvar);
static CONDVAR_DECL(ir_right_obstacle_topic_condvar);
static CONDVAR_DECL(ir_left_obstacle_topic_condvar);

#define IR_THREAD_PRIORITY (NORMALPRIO + 10) // Higher priority for obstacle detection
#define OBSTACLE_THRESHOLD 200  // Threshold for obstacle detection

static THD_WORKING_AREA(waIRSensorThread, 256);
static THD_FUNCTION(IRSensorThread, arg) {
    (void)arg;
    chRegSetThreadName("IRSensorReading");
    unsigned int front_value, right_value, left_value;

    while (true) {
        // Fetch data using the proximity API
        front_value = get_proximity(FRONT_SENSOR_CHANNEL);
        right_value = get_proximity(RIGHT_SENSOR_CHANNEL);
        left_value = get_proximity(LEFT_SENSOR_CHANNEL);

        // Check each sensor and publish a message if an obstacle is detected
        if (front_value > OBSTACLE_THRESHOLD) {
            messagebus_topic_publish(&ir_front_obstacle_topic, &front_value, sizeof(front_value));
        }
        if (right_value > OBSTACLE_THRESHOLD) {
            messagebus_topic_publish(&ir_right_obstacle_topic, &right_value, sizeof(right_value));
        }
        if (left_value > OBSTACLE_THRESHOLD) {
            messagebus_topic_publish(&ir_left_obstacle_topic, &left_value, sizeof(left_value));
        }

        chThdSleepMilliseconds(100);  // Sensor polling rate
    }
}

void ir_module_init(void) {
    static THD_WORKING_AREA(waIRSensorThread, 1024); // Increase stack size if needed
    chThdCreateStatic(waIRSensorThread, sizeof(waIRSensorThread), IR_THREAD_PRIORITY, IRSensorThread, NULL);
    
    messagebus_topic_init(&ir_front_obstacle_topic, &ir_front_obstacle_topic_lock, &ir_front_obstacle_topic_condvar, NULL, 0);
    messagebus_advertise_topic(&bus, &ir_front_obstacle_topic, "/ir_front_obstacle");

    messagebus_topic_init(&ir_right_obstacle_topic, &ir_right_obstacle_topic_lock, &ir_right_obstacle_topic_condvar, NULL, 0);
    messagebus_advertise_topic(&bus, &ir_right_obstacle_topic, "/ir_right_obstacle");

    messagebus_topic_init(&ir_left_obstacle_topic, &ir_left_obstacle_topic_lock, &ir_left_obstacle_topic_condvar, NULL, 0);
    messagebus_advertise_topic(&bus, &ir_left_obstacle_topic, "/ir_left_obstacle");

    chThdCreateStatic(waIRSensorThread, sizeof(waIRSensorThread), NORMALPRIO, IRSensorThread, NULL);
}
