/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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

/**
 * @file
 * @ingroup grove 
 * @brief Digital Input
 * 
 * Read a digital value from an input pin using the IO mraa library.
 *
 * @date 13/06/2015
 */

#include "mraa.hpp"

#include <iostream>
#include <unistd.h>

/*
 * Digital input example
 *
 * Demonstrate how to read a digital value from an input pin using the MRAA
 * library.
 * Suitable ones in the Grove Starter Kit are the Button and Touch Sensor.
 *
 * - digital in: input connected to digital pin 4 (Grove Base Shield Port D4)
 *
 * Additional linker flags: none
 */

int main()
{
	// check that we are running on Galileo or Edison
	mraa_platform_t platform = mraa_get_platform_type();
	if ((platform != MRAA_INTEL_GALILEO_GEN1) &&
			(platform != MRAA_INTEL_GALILEO_GEN2) &&
			(platform != MRAA_INTEL_EDISON_FAB_C)) {
		std::cerr << "Unsupported platform, exiting" << std::endl;
		return MRAA_ERROR_INVALID_PLATFORM;
	}

	// create a GPIO object from MRAA using pin 4
	mraa::Gpio* d_pin = new mraa::Gpio(4);
	if (d_pin == NULL) {
		std::cerr << "Can't create mraa::Gpio object, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}

	// set the pin as input
	if (d_pin->dir(mraa::DIR_IN) != MRAA_SUCCESS) {
		std::cerr << "Can't set digital pin as input, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}

	// loop forever printing the digital input value every second
	for (;;) {
		int pin_value = d_pin->read();
		std::cout << "value " << pin_value << std::endl;
		sleep(1);
	}

	return MRAA_SUCCESS;
}
