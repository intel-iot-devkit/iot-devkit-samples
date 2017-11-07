/*
 * Author: Jessica Gomez <jessica.gomez.hernandez@intel.com>
 * Copyright (c) 2015 - 2017 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* On-board LED Blink
 * Turns the on-board LED on for one second, then off for one second, repeatedly.
 * Note: You need to change the LED_GPIO below to the GPIO pin number that matches your platform
 * UP2 Board - pin 13
 * UP2 Board with Grove Pi Shield - Port offset by 512 i.e. 512 + 3 for D3 will be 515. 
 * Minnowboard Turbot - pin 104
 * Intel Galileo Gen 2 - pin 13
 * Intel Edison - pin 13
 * Intel Joule with expansion board - pin 101
*/
#include <mraa.h>
#include <stdio.h>
#include <unistd.h>
//Change the pin number according to board
#define LED_GPIO 13

int main()
{
	mraa_gpio_context d_pin = mraa_gpio_init(LED_GPIO);
	//Comment this if using the Grove PI shield
	mraa_add_subplatform(MRAA_GROVEPI, "0");
	if (d_pin == NULL) {
		fprintf(stderr, "MRAA couldn't initialize GPIO, exiting");
		return MRAA_ERROR_UNSPECIFIED;
	}

	// set the pin as output
	if (mraa_gpio_dir(d_pin, MRAA_GPIO_OUT) != MRAA_SUCCESS) {
		fprintf(stderr, "Can't set digital pin as output, exiting");
		return MRAA_ERROR_UNSPECIFIED;
	};

	// loop forever toggling the on board LED every second
	for (;;) {
		mraa_gpio_write(d_pin, 0);
		sleep(1);
		mraa_gpio_write(d_pin, 1);
		sleep(1);
	}

	return MRAA_SUCCESS;
}
