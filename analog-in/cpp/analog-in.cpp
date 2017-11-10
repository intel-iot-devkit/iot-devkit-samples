/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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

/* Analog input
 * Read values from a gpio analog input pin.
 */

#include <mraa.hpp>
#include <iostream>
#include <unistd.h>

// Define the following if using a Grove Pi Shield
#define USING_GROVE_PI_SHIELD
using namespace std;
using namespace mraa;

int main()
{
	int gpioPin = 13;
	string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
    		"you are running it on an unrecognized platform. "
			"You may need to modify the MRAA/UPM initialization code to "
			"ensure it works properly on your platform.\n\n";

	// check which board we are running on
	Platform platform = getPlatformType();
	switch (platform) {
		case INTEL_UP2:
#ifdef USING_GROVE_PI_SHIELD
			gpioPin = 2 + 512; // A2 Connector (512 offset needed for the shield)
			break;
#endif
		default:
	        cerr << unknownPlatformMessage;
	}
#ifdef USING_GROVE_PI_SHIELD
	addSubplatform(GROVEPI, "0");
#endif
	// check if running as root
	int euid = geteuid();
	if (euid) {
		cerr << "This project uses Mraa I/O operations, but you're not running as 'root'.\n"
				"The IO operations below might fail.\n"
				"See the project's Readme for more info.\n\n";
	}

	// create an analog input object from MRAA using the pin
	Aio* a_pin = new Aio(gpioPin);
	if (a_pin == NULL) {
		cerr << "Can't create mraa::Aio object, exiting" << endl;
		return MRAA_ERROR_UNSPECIFIED;
	}

	// loop forever printing the input value every second
	for (;;) {
		uint16_t pin_value = a_pin->read();
		cout << "analog input value " << pin_value << endl;
		sleep(1);
	}

	return SUCCESS;
}
