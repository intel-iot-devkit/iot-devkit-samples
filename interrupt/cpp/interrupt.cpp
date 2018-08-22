/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
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

/**
 * Demonstrate how to react on an external event with an ISR (Interrupt Service
 * Routine), which will run independently of the main program flow using the
 * MRAA library.
 * Any button or sensor that can generate a voltage transition from ground to
 * Vcc or viceversa can be used with this example code.
 * Suitable ones in the Grove Starter Kit are the Button and Touch Sensor,
 * connected to a digital pin
 *
 * Use a platform with GPIO Interrupt capabilities
 */

#include <mraa.hpp>

#include <iostream>
#include <unistd.h>

using namespace std;
using namespace mraa;

// counter that will be updated by the interrupt routine
static volatile int counter = 0;

// ISR, update the value of the counter
void interrupt(void * args) {
    ++counter;
}

// leave warning/error message in console and wait for user to press Enter
void inputEnter(const string& str)
{
    cerr << str << endl << "Press Enter to continue..." << endl;
    cin.get();
}

// check if running as root
void checkRoot(void)
{
    int euid = geteuid();
    if (euid) {
        inputEnter("This project uses Mraa I/O operations that require\n"
            "'root' privileges, but you are running as non - root user.\n"
            "Passwordless keys(RSA key pairs) are recommended \n"
            "to securely connect to your target with root privileges. \n"
            "See the project's Readme for more info.\n\n");
    }
    return;
}

// set pin values depending on the current board (platform)
void initPlatform(int& gpioPin)
{
    
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_MINNOWBOARD_MAX: // Same for Minnowboard Turbot
    case INTEL_JOULE_EXPANSION:
        gpioPin = 26;
        break;
    case UNKNOWN_PLATFORM:
    {
        string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n";
        inputEnter(unknownPlatformMessage);
        break;
    }
    default:
        break;
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
        inputEnter("Can't create mraa::Gpio object, exiting");
        return MRAA_ERROR_UNSPECIFIED;
    }

    // set the pin as input
    if (d_pin->dir(DIR_IN) != SUCCESS) {
        inputEnter("Can't set digital pin as input, exiting");
        return MRAA_ERROR_UNSPECIFIED;
    }

    // set the ISR, it will be executed on both edges
    if (d_pin->isr(EDGE_BOTH, interrupt, NULL) != SUCCESS) {
        inputEnter("Can't assign ISR to pin, exiting");
        return ERROR_UNSPECIFIED;
    }

    // loop forever printing the counter value every second
    for (;;) {
        cout << "counter value " << counter << endl;
        sleep(1);
    }

    return SUCCESS;
}
