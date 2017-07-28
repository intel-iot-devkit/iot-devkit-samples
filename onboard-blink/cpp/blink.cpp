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

 /**
  * TODO use a platform with GPIO capabilities and an onboard LED
 */

#include <mraa.hpp>

#include <iostream>
#include <unistd.h>

int main()
{
  //TODO Change the GPIO to one that matches your platform
	// Intel Galileo Gen 2 = 13
	// Intel Edison = 13
	// Intel Joule with expansion board = 100
  mraa::Gpio* d_pin = new mraa::Gpio(13, true, false);;

	// set the pin as output
	if (d_pin->dir(mraa::DIR_OUT) != mraa::SUCCESS) {
		std::cerr << "Can't set digital pin as output, exiting" << std::endl;
		return MRAA_ERROR_UNSPECIFIED;
	}

	// loop forever toggling the on board LED every second
	for (;;) {
		d_pin->write(1);
		sleep(1);
		d_pin->write(0);
		sleep(1);
	}

	return mraa::SUCCESS;
}
