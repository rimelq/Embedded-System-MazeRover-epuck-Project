#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ch.h>
#include <hal.h>
#include <memory_protection.h>
#include <usbcfg.h>
#include <motors.h>
#include <camera/po8030.h>
#include <chprintf.h>

// In order to be able to use the RGB LEDs and User button
// These funtcions are handled by the ESP32 and the communication with the uC is done via SPI
#include <spi_comm.h>

#include "main.h"
#include "pi_regulator.h"
#include "process_image.h"

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
    //starts the camera
    dcmi_start();
	po8030_start();
	//inits the motors
	motors_init();

	//starts RGB LEDS and User button managment
	spi_comm_start();

	//init color detection mode: see process_image.h for values
	//the color detection can be controled from plotImage Python code
	select_color_detection(GREEN_COLOR);

	//disable motors by default. Can be enable from plotImage Python code
	set_enabled_motors(false);

	//stars the threads for the pi regulator and the processing of the image
	pi_regulator_start();
	process_image_start();

    /* Infinite loop: used here for handling control commands sent from plotImage Python code */
    while (1) {
		// read control commands
        volatile uint8_t ctrl_cmd = chSequentialStreamGet((BaseSequentialStream *) &SD3);
		switch (ctrl_cmd) {
			case 'R':
			case 'r':
				select_color_detection(RED_COLOR);
				break;
			case 'G':
			case 'g':
				select_color_detection(GREEN_COLOR);
				break;
			case 'B':
			case 'b':
				select_color_detection(BLUE_COLOR);
				break;
			case 'M':
			case 'm':
				toogle_enabled_motors();
				break;
		}
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
