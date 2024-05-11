#include "ch.h"
#include "hal.h"
#include <msgbus/messagebus.h>

#include "imu_module.h"
#include "motor_module.h"


// Initialize message bus topics and static variables
extern messagebus_t bus;
static uint8_t orientation = NO_TILT;
static bool start_function = DISABLE_IMU;


// IMU thread: get maze tilt direction, send result to motor controller thread
static THD_WORKING_AREA(waIMUThread, 256);
static THD_FUNCTION(IMUThread, arg) {
    (void)arg;
    chRegSetThreadName("IMUReading");

    messagebus_topic_t *imu_topic = messagebus_find_topic_blocking(&bus, "/imu");
    imu_msg_t imu_values;


    while (true) {
        // temporary variable to check tilt direction before sending to motor thread
        uint8_t temp = NO_TILT;  

        // check if IMU function should start
        start_function = get_start_imu();
        
        if (start_function == ENABLE_IMU) {  // case: motor thread activated IMU
            // get IMU measured value
            messagebus_topic_wait(imu_topic, &imu_values, sizeof(imu_values));  
            temp = show_gravity(&imu_values);
            if ((temp == RIGHT_TILT) || (temp == LEFT_TILT)) {
                orientation = temp;  // write onto static variable for motor thread
            }
        }
        else if (start_function == DISABLE_IMU) { //case: motor thread deactivated IMU
            orientation = NO_TILT;  // reset static variable for future event
        }
        
        chThdSleepMilliseconds(50);  // sleep to give hand to other module
    }
}


// Function that starts the thread
void imu_module_start(void) {
    chThdCreateStatic(waIMUThread, sizeof(waIMUThread), NORMALPRIO+2, 
    IMUThread, NULL);
}

// Function to detect gravity orientation and turn the right LED on
uint8_t show_gravity(imu_msg_t *imu_values){

    uint8_t led3 = 0, led7 = 0;
    uint8_t calculated_orientation = NO_TILT;
    float threshold = 2.0;  // threshold for maze tilt angle
    float *accel = imu_values->acceleration;  //create a pointer to the array

    chSysLock();

    //we find which led of each axis should be turned on
    if(accel[X_AXIS] > threshold) {  // case: tilted to the Left
        led7 = 1;
        calculated_orientation = LEFT_TILT;
    }
    else if(accel[X_AXIS] < -threshold) {  // case: tilted to the Right
        led3 = 1;
        calculated_orientation = RIGHT_TILT;
    }
    else {  // case: no tilt detected
        calculated_orientation = NO_TILT;
    }

    chSysUnlock();

    //Turn ON Leds
    palWritePad(GPIOD, GPIOD_LED3, led3 ? 0 : 1);
    palWritePad(GPIOD, GPIOD_LED7, led7 ? 0 : 1);

    return calculated_orientation;
}

// Function that sends the orientation to the Motor thread
uint8_t get_orientation_motor(void) {
    return orientation;
}