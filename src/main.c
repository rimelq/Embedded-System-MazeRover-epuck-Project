#include <ch.h>
#include <hal.h>
#include <memory_protection.h>
#include <motors.h>
#include "i2c_bus.h"

#include "main.h"
#include "ir_module.h"  
#include "imu_module.h"
#include "motor_module.h"


// Global declaration of the message bus
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);


int main(void)
{
	// START NECESSARY DEPENDENCIES AND THREADS
    halInit();
    chSysInit();
    mpu_init();
	messagebus_init(&bus, &bus_lock, &bus_condvar);  //initialize the message bus
	i2c_start();
    
    // INITIALIZE ALL MODULES
	motors_init();
	proximity_start();
	imu_start();

	// START ALL THREADS MODULES: to launch the different threads
	ir_module_start();
	motor_controller_start();
	imu_module_start();

	
    // INFINITE LOOP MAIN //
    while (true) {
    	// NOTHING TO DO
		chThdSleepMilliseconds(200);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}