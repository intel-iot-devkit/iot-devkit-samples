/*
 * Author: Jessica Gomez <jessica.gomez.hernandez@intel.com>
 * Copyright (c) 2015 - 2016 Intel Corporation.
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

#include <mraa.h>

#include <stdio.h>
#include <unistd.h>

int main()
{
	// select onboard LED pin based on the platform type
	// create a GPIO object from MRAA using it
	mraa_platform_t platform = mraa_get_platform_type();
	mraa_gpio_context d_pin = NULL;
	switch (platform) {
		case MRAA_INTEL_GALILEO_GEN1:
			d_pin = mraa_gpio_init_raw(3);
			break;
		case MRAA_INTEL_GALILEO_GEN2:
			d_pin = mraa_gpio_init(13);
			break ;
		case MRAA_INTEL_EDISON_FAB_C:
			d_pin = mraa_gpio_init(13);
			break;
		case MRAA_INTEL_JOULE_EXPANSION:
			d_pin = mraa_gpio_init(100);
			break;
		default:
			fprintf(stderr, "Unsupported platform, exiting");
			return MRAA_ERROR_INVALID_PLATFORM;
	}
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
