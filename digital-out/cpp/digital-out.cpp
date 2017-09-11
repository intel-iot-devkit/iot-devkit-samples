/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
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

/**
 * Demonstrate how to write a digital value to an output pin using the MRAA
 * library.
 * A suitable part to use this example with in the Grove Starter Kit is the LED.
 * Output connected to digital pin 8 (Grove Base Shield Port D8)
 * TODO Use a platform with GPIO capabilities
 */

#include <mraa.hpp>

#include <iostream>
#include <unistd.h>

int main()
{
	// create a gpio object from MRAA using pin 8
	mraa::Gpio* d_pin = new mraa::Gpio(8);
	if(d_pin == NULL) {
		std::cerr << "Can't create mraa::Gpio object, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// set the pin as output
	if (d_pin->dir(mraa::DIR_OUT) != mraa::SUCCESS) {
		std::cerr << "Can't set digital pin as output, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// loop forever toggling the digital output every second
	for(;;) {
		d_pin->write(0);
		sleep(1);
		d_pin->write(1);
		sleep(1);
	}

	return mraa::SUCCESS;
}
