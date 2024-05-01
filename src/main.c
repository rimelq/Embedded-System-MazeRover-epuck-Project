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

// In order to be able to use the RGB LEDs and User button
// These funtcions are handled by the ESP32 and the communication with the uC is done via SPI
#include <spi_comm.h>

#include "main.h"
#include "pi_regulator.h"
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
    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //start the USB communication
    usb_start();
	//inits the motors
	motors_init();

	//starts RGB LEDS and User button managment
	spi_comm_start();

	//enable motors by default. Can be chnaged from plotImage Python code
	set_enabled_motors(true);

	//stars the threads for the pi regulator
	pi_regulator_start();

	//initialize the message bus
    messagebus_init(&bus, &bus_lock, &bus_condvar);

    //initialize modules
    ir_module_init();  //initialize the IR sensor module
    imu_module_init();  //initialize the IMU module
	navigation_module_init(); //initialize navigation module

    /* Infinite loop */
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
