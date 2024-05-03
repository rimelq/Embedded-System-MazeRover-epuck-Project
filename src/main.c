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
    serial_start();  //starts the serial communication
    usb_start();  //start the USB communication
	spi_comm_start();  //starts RGB LEDS and User button managment
    messagebus_init(&bus, &bus_lock, &bus_condvar);  //initialize the message bus

    // INITIALIZE ALL MODULES: to create all topics
    ir_module_init();
	navigation_module_init();
	imu_module_init();
	motor_controller_init();

	// START ALL THREADS MODULES: to launch the different threads
	ir_module_start();
	navigation_module_start();
	imu_module_start();
	motor_controller_start();

    // INFINITE LOOP MAIN //
    while (1) {
    	//waits 0.2 second
		chThdSleepMilliseconds(200);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
