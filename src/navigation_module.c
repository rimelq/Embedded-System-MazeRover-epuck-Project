#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include "navigation_module.h"

#define NAVIGATION_THREAD_PRIORITY (NORMALPRIO + 5) // Lower priority than the IR sensor thread

extern messagebus_t bus;

static THD_WORKING_AREA(waNavigationThread, 512);
static THD_FUNCTION(NavigationThread, arg) {
    //(void)p;   A CHECK!!
    
    unsigned int front_obstacle_distance, right_obstacle_distance, left_obstacle_distance;

    // Find topics
    messagebus_topic_t *front_topic = messagebus_find_topic_blocking(&bus, "/ir_front_obstacle");
    messagebus_topic_t *right_topic = messagebus_find_topic_blocking(&bus, "/ir_right_obstacle");
    messagebus_topic_t *left_topic = messagebus_find_topic_blocking(&bus, "/ir_left_obstacle");

    while (true) {
        // Wait for messages on each topic
        messagebus_topic_wait(front_topic, &front_obstacle_distance, sizeof(front_obstacle_distance));     // A CHANGER CAR BLOQUE
            // Process front obstacle
            //handle_front_obstacle(front_obstacle_distance);
        
        messagebus_topic_wait(right_topic, &right_obstacle_distance, sizeof(right_obstacle_distance)); 
            // Process right obstacle
            //handle_right_obstacle(right_obstacle_distance);
        
        messagebus_topic_wait(left_topic, &left_obstacle_distance, sizeof(left_obstacle_distance)); 
            // Process left obstacle
            //handle_left_obstacle(left_obstacle_distance);
    }
}

void navigation_module_init(void) {
    static THD_WORKING_AREA(waNavigationThread, 1024); // Assuming Navigation processing might need a larger stack
    chThdCreateStatic(waNavigationThread, sizeof(waNavigationThread), NAVIGATION_THREAD_PRIORITY, NavigationThread, NULL);
}
