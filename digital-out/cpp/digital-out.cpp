/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
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

/* Digital output
 * Write values to a gpio digital output pin.
 */

#include <mraa.hpp>
#include <iostream>
#include <unistd.h>

// Define the following if using a Grove Pi Shield
#define USING_GROVE_PI_SHIELD
using namespace std;
using namespace mraa;

// check if running as root
void checkRoot(void)
{
    int euid = geteuid();
    if (euid) {
        cerr << "This project uses Mraa I/O operations, but you're not running as 'root'.\n"
                "The IO operations below might fail.\n"
                "See the project's Readme for more info.\n\n";
    }
    return;
}

void initPlatform(int& gpioPin)
{
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_UP2:
#ifdef USING_GROVE_PI_SHIELD
        gpioPin = 4 + 512; // D4 Connector (512 offset needed for the shield)
        break;
#endif
    case INTEL_UP:
    case INTEL_EDISON_FAB_C:
    case INTEL_GALILEO_GEN2:
        break;
    case INTEL_MINNOWBOARD_MAX: // Same for Minnowboard Turbot
        gpioPin = 104;
        break;
    case INTEL_JOULE_EXPANSION:
        gpioPin = 101;
        break;
    default:
        string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n\n";
        cerr << unknownPlatformMessage;
    }
    return;
}

int main()
{
    // check if running as root
    checkRoot();

    int gpioPin = 13;
    initPlatform(gpioPin);
    
#ifdef USING_GROVE_PI_SHIELD
    addSubplatform(GROVEPI, "0");
#endif

    // create a GPIO object from MRAA for the pin
    Gpio* d_pin = new Gpio(gpioPin);
    if (d_pin == NULL) {
        cerr << "MRAA couldn't initialize GPIO, exiting." << endl;
        return MRAA_ERROR_UNSPECIFIED;
    }
    // set the pin as output
    if (d_pin->dir(DIR_OUT) != SUCCESS) {
        cerr << "Can't set digital pin as output, exiting" << endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

    // loop forever toggling the digital output every second
    for(;;) {
        if (d_pin->write(0) != SUCCESS) {
            cerr << "MRAA cannot write pin value!" << endl;
            return MRAA_ERROR_UNSPECIFIED;
        }
        sleep(1);     
        if (d_pin->write(1) != SUCCESS) {
            cerr << "MRAA cannot write pin value!" << endl;
            return MRAA_ERROR_UNSPECIFIED;
        }
        sleep(1);
    }

    return SUCCESS;
}
