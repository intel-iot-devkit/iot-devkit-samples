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

/* PWM
 * Write values to a gpio digital output pin.
 */

#include <mraa.hpp>
#include <iostream>
#include <unistd.h>

#define USING_GROVE_PI_SHIELD
using namespace std;
using namespace mraa;

// leave warning/error message in console and wait for user to press Enter
void consoleMessage(const string& str)
{
    cerr << str << endl;
    sleep(10);
}

// check if running as root
void checkRoot(void)
{
    int euid = geteuid();
    if (euid) {
        consoleMessage("This project uses Mraa I/O operations that require\n"
            "'root' privileges, but you are running as non - root user.\n"
            "Passwordless keys(RSA key pairs) are recommended \n"
            "to securely connect to your target with root privileges. \n"
            "See the project's Readme for more info.\n\n");
    }
    return;
}

// set pin values depending on the current board (platform)
void initPlatform(int& pwmPin)
{
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_UP2:
#ifdef USING_GROVE_PI_SHIELD
        pwmPin = 5 + 512; // D5 works as PWM on Grove PI Shield 
        break;
#endif			
        break;
    default:
        string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n\n";
        consoleMessage(unknownPlatformMessage);
    }
    return;
}

int main()
{
    // check if running as root
    checkRoot();
    int pwmPin = 33;
    initPlatform(pwmPin);

#ifdef USING_GROVE_PI_SHIELD
    addSubplatform(GROVEPI, "0");
#endif

    // note that not all digital pins can be used for PWM, the available ones
    // are usually marked with a ~ on the board's silk screen
    Pwm* pwm_pin = new Pwm(pwmPin);
    if (pwm_pin == NULL) {
        consoleMessage("Can't create mraa::Pwm object, exiting");
        return MRAA_ERROR_UNSPECIFIED;
    }

    // enable PWM on the selected pin
    if (pwm_pin->enable(true) != SUCCESS) {
        consoleMessage("Cannot enable PWM on mraa::PWM object, exiting");
        return MRAA_ERROR_UNSPECIFIED;
    }

    // PWM duty_cycle value, 0.0 == 0%, 1.0 == 100%
    float duty_cycle = 0.0;

    // select a pulse width period of 1ms
    int period = 1;

    // loop forever increasing the PWM duty cycle from 0% to 100%,
    // a full cycle will take ~5 seconds
    for (;;) {
        pwm_pin->pulsewidth_ms(period);
        if (pwm_pin->write(duty_cycle) != SUCCESS) {
            consoleMessage("MRAA cannot write duty cycle!");
            return ERROR_UNSPECIFIED;
        }
        usleep(50000);
        duty_cycle += 0.01;
        if(duty_cycle > 1.0) {
            duty_cycle = 0.0;
        }
    }

    return SUCCESS;
}
