#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ch.h>
#include <hal.h>
#include <memory_protection.h>
#include <usbcfg.h>
#include <motors.h>
#include <chprintf.h>
#include <spi_comm.h>

#include "main.h"
#include "ir_module.h"  
#include "imu_module.h"
#include "motor_module.h"
#include "navigation_module.h"

// Global declaration of the message bus
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);


void SendUint8ToComputer(uint8_t* data, uint16_t size) 
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

int main(void)
{
	// START NECESSARY DEPENDENCIES AND THREADS
    halInit();
    chSysInit();
    mpu_init();
	messagebus_init(&bus, &bus_lock, &bus_condvar);  //initialize the message bus
    serial_start();  //starts the serial communication
    usb_start();  //start the USB communication
	spi_comm_start();  //starts RGB LEDS and User button managment
	i2c_start();
    

    // INITIALIZE ALL MODULES: to create all topics
    //ir_module_init();
	//navigation_module_init();
	//motor_controller_init();
	//imu_module_init();
	motors_init();
    //imu_start();
	proximity_start();

	// START ALL THREADS MODULES: to launch the different threads
	ir_module_start();
	//navigation_module_start();
	motor_controller_start();
	//imu_module_start();

	
    // INFINITE LOOP MAIN //
    while (true) {
    	// NOTHING TO DO: already gave hand to Round Robin
		
		chThdSleepMilliseconds(200);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
