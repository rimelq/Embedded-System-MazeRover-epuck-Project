#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>
#include <sensors/imu.h>
#include "imu_module.h"
#include "i2c_bus.h"


// Initialize message bus topics
extern messagebus_t bus;
// messagebus_topic_t imu_orientation_topic;
// static MUTEX_DECL(imu_orientation_topic_lock);
// static CONDVAR_DECL(imu_orientation_topic_condvar);


// IMU module thread: detect maze orientation, turn correct LED, send orientation to navigation module
static THD_WORKING_AREA(waIMUThread, 256);
static THD_FUNCTION(IMUThread, arg) {
    (void)arg;
    chRegSetThreadName("IMUReading");

    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
    imu_msg_t imu_values;
    bool orientation, start_function; 

    // Find message bus topics
    // messagebus_topic_t *wakeup = messagebus_find_topic_blocking(&bus, "/imu_wake_up");
    //imu_compute_offset(imu_topic, 200);

    while (true) {
        
        //wait for new measures to be published
        messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));

        // WAIT FOR WAKE UP SIGNAL FROM NAVIGATION MODULE
        //messagebus_topic_wait(wakeup, &start_function, sizeof(start_function));
        // if (start_function) {
            // Measure tilt with IMU, turn on the LED, get new robot direction
            // orientation = show_gravity(&imu_values);
            // chThdSleepMilliseconds(100);
        orientation = show_gravity(&imu_values);
        chThdSleepMilliseconds(100);

            // publish new orientation to topic for navigation module
            // messagebus_topic_publish(&imu_orientation_topic, &orientation, sizeof(orientation));
            // start_function = false;
        //}
        // Gives back hand to the Round Robin thread (-> Navigation thread where left off)
    }
}

// Function to initialise thread and created topics
void imu_module_init(void) {
    // messagebus_topic_init(&imu_orientation_topic, &imu_orientation_topic_lock, &imu_orientation_topic_condvar, NULL, 0);
    // messagebus_advertise_topic(&bus, &imu_orientation_topic, "/imu_orientation");
}

// Function that starts the thread
void imu_module_start(void) {
    chThdCreateStatic(waIMUThread, sizeof(waIMUThread), NORMALPRIO+2, IMUThread, NULL);
}

// Function to detect gravity orientation and turn the right LED on
bool show_gravity(imu_msg_t *imu_values){

    uint8_t led3 = 0, led7 = 0;
    bool calculated_orientation = true;
    float threshold = 3.0;
    float *accel = imu_values->acceleration;  //create a pointer to the array for shorter name

    chSysLock();

    //we find which led of each axis should be turned on
    if(accel[X_AXIS] > threshold)
        led7 = 1;
    else if(accel[X_AXIS] < -threshold)
        led3 = 1;

    chSysUnlock();

    //we invert the values because a led is turned on if the signal is low
    palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
    palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

    return calculated_orientation;
}