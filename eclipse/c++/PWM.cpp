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

#include "mraa.hpp"

#include <iostream>
#include <unistd.h>

/**
 * @file
 * @ingroup grove
 * @brief Pulse Width Modulation
 *
 * Demonstrate how to use PWM with an output pin using the MRAA library.
 * If the output is connected to a led, its intensity, as perceived by the human
 * eye, will vary depending on the duty cycle.
 * A suitable part to use this example with in the Grove Starter Kit is the LED.
 *
 * @hardware Sensors used:\n
 * - digital out: output connected to digital pin 6 (Grove Base Shield Port D6)
 *
 * @date 07/06/2015
 */
int main()
{
	// check that we are running on Galileo or Edison
	mraa::Platform platform = mraa::getPlatformType();
	if ((platform != mraa::INTEL_GALILEO_GEN1)
	    && (platform != mraa::INTEL_GALILEO_GEN2)
	    && (platform != mraa::INTEL_EDISON_FAB_C)) {
	  std::cerr << "Unsupported platform, exiting" << std::endl;
	  return mraa::ERROR_INVALID_PLATFORM;
	}

	// create a GPIO object from MRAA using pin 6
	// note that not all digital pins can be used for PWM, the available ones
	// are usually marked with a ~ on the board's silk screen
	mraa::Pwm* pwm_pin = new mraa::Pwm(6);
	if (pwm_pin == NULL) {
		std::cerr << "Can't create mraa::Pwm object, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// enable PWM on the selected pin
	if (pwm_pin->enable(true) != mraa::SUCCESS) {
		std::cerr << "Cannot enable PWM on mraa::PWM object, exiting" << std::endl;
		return mraa::ERROR_UNSPECIFIED;
	}

	// PWM duty_cycle value, 0.0 == 0%, 1.0 == 100%
	float duty_cycle = 0.0;

	// select a pulse width period of 1ms
	int period = 1;

	// loop forever increasing the PWM duty cycle from 0% to 100%,
	// a full cycle will take ~5 seconds
	for (;;) {
		pwm_pin->pulsewidth_ms(period);
		pwm_pin->write(duty_cycle);
		usleep(50000);
		duty_cycle += 0.01;
		if(duty_cycle > 1.0) {
			duty_cycle = 0.0;
		}
	}

	return mraa::SUCCESS;
}
