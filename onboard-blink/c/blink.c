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

/* On-board LED Blink
 * Turns the on-board LED on for one second, then off for one second, repeatedly.
 */

#include <mraa.h>
#include <stdio.h>
#include <unistd.h>

// Define the following if using a Grove Pi Shield
#define USING_GROVE_PI_SHIELD

int main()
{
    int gpioPin = 13;
    char unknownPlatformMessage[] ="This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n\n";

    // check which board we are running on
    mraa_platform_t platform = mraa_get_platform_type();
    switch (platform) {
        case MRAA_UP2:
#ifdef USING_GROVE_PI_SHIELD
            gpioPin = 4 + 512; // D4 Connector (512 offset needed for the shield)
            break;
#endif
        case MRAA_UP:
        case MRAA_INTEL_EDISON_FAB_C:
        case MRAA_INTEL_GALILEO_GEN2:
            break;
        case MRAA_INTEL_MINNOWBOARD_MAX: // Same for Minnowboard Turbot
            gpioPin = 104;
            break;
        case MRAA_INTEL_JOULE_EXPANSION:
            gpioPin = 101;
            break;
        default:
            fprintf(stderr, "%s", unknownPlatformMessage);
    }
#ifdef USING_GROVE_PI_SHIELD
    mraa_add_subplatform(MRAA_GROVEPI, "0");
#endif
    // check if running as root
    int euid = geteuid();
    if (euid) {
        fprintf(stderr, "This project uses Mraa I/O operations, but you're not running as 'root'.\n"
                "The IO operations below might fail.\n"
                "See the project's Readme for more info.\n\n");
    }
    // create the pin object
    mraa_gpio_context d_pin = mraa_gpio_init(gpioPin);
    if (d_pin == NULL) {
        fprintf(stderr, "MRAA couldn't initialize GPIO, exiting.\n");
        return MRAA_ERROR_UNSPECIFIED;
    }

    // set the pin as output
    if (mraa_gpio_dir(d_pin, MRAA_GPIO_OUT) != MRAA_SUCCESS) {
        fprintf(stderr, "Can't set digital pin as output, exiting.\n");
        return MRAA_ERROR_UNSPECIFIED;
    };

    // loop forever toggling the on board LED every second
    for (;;) {
        if (mraa_gpio_write(d_pin, 0) != MRAA_SUCCESS) {
            fprintf(stderr, "MRAA cannot write pin value!\n");
            return MRAA_ERROR_UNSPECIFIED;
        }
        sleep(1);
        if (mraa_gpio_write(d_pin, 1) != MRAA_SUCCESS) {
            fprintf(stderr, "MRAA cannot write pin value!\n");
            return MRAA_ERROR_UNSPECIFIED;
        }
        sleep(1);
    }

    return MRAA_SUCCESS;
}
