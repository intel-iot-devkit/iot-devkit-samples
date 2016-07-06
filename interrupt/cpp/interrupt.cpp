/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
 * @file
 * @ingroup grove 
 * @brief ISR, interrupt service routine
 * 
 * Demonstrate how to react on an external event with an ISR (Interrupt Service
 * Routine), which will run independently of the main program flow using the
 * MRAA library.
 * Any button or sensor that can generate a voltage transition from ground to
 * Vcc or viceversa can be used with this example code.
 * Suitable ones in the Grove Starter Kit are the Button and Touch Sensor,
 * connected to digital pin 4 (Grove Base Shield Port D4)
 *
 * @date 29/09/2015
 */
 
#include <mraa.hpp>

#include <iostream>
#include <unistd.h>

// counter that will be updated by the interrupt routine
static volatile int counter = 0;

// ISR, update the value of the counter
void interrupt(void * args) {
	++counter;
}

int main()
{
	// check that we are running on Galileo or Edison
	mraa::Platform platform = mraa::getPlatformType();
	if ((platform != mraa::INTEL_GALILEO_GEN1) &&
			(platform != mraa::INTEL_GALILEO_GEN2) &&
			(platform != mraa::INTEL_EDISON_FAB_C)) {
		std::cerr << "Unsupported platform, exiting" << std::endl;
		return mraa::ERROR_INVALID_PLATFORM;
	}

	// create a GPIO object from MRAA using pin 4
	mraa::Gpio* d_pin = new mraa::Gpio(4);
	if (d_pin == NULL) {
		std::cerr << "Can't create mraa::Gpio object, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// set the pin as input
	if (d_pin->dir(mraa::DIR_IN) != mraa::SUCCESS) {
		std::cerr << "Can't set digital pin as input, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// set the ISR, it will be executed on both edges (on Galileo Gen 1 only
	// this mode is supported)
	if (d_pin->isr(mraa::EDGE_BOTH, interrupt, NULL) != mraa::SUCCESS) {
		std::cerr << "Can't assign ISR to pin, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// loop forever printing the counter value every second
	for (;;) {
		std::cout << "counter value " << counter << std::endl;
		sleep(1);
	}

	return mraa::SUCCESS;
}
