/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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

#include <stdio.h>
#include <string.h>

// Remember that mraa is not like the above headers, it requires you to link
// against the dynamic library libmraa.so
#include "mraa.h"

int
main(int argc, char **argv)
{
    // mraa functions and data types are always prefixed with mraa_ so you know
    // what you are using. Enums are upper case MRAA_

    // alot of libmraa functions return mraa_result_t which is a return code. 0
    // or MRAA_SUCCESS is what you would expect as a success code. Typically
    // these may give you an idea of what went wrong although for debugging
    // checking syslog (note syslog gets redirected to systemd's journal on
    // alot of platforms) will be more useful
    mraa_result_t result = MRAA_SUCCESS;

    // mraa init is the library initialisation call, it is called on library
    // load for you however it's a good idea to run it again to make sure your
    // board initialised correctly.
    result = mraa_init();
    if (result != MRAA_SUCCESS) {
        // this call will print the mraa error code to stdout
        mraa_result_print(result);
        return result;
    }

    // The mraa initialisation will give us a number of information about our
    // board and the library running
    char* platform_name = mraa_get_platform_name();
    mraa_platform_t platform_type = mraa_get_platform_type();
    char* version = mraa_get_version();

    fprintf(stdout, "hello mraa\n Version: %s\n Running on %s (%d)\n",
            version, platform_name, platform_type);

    // Let's use a GPIO now, mraa requires you to store contexts to use IO
    // which represent IOs whether GPIOs, i2c buses or anything else which you
    // create by calling mraa_*_init() functions. These functions allocate
    // memory but return an opaque pointer which you cannot view the content
    // of. However before doing this it's considered good practice to check our
    // platform can access this IO as a GPIO.
    if (mraa_pin_mode_test(13, MRAA_PIN_GPIO)) {

        mraa_gpio_context mygpio = mraa_gpio_init(13);
        // _init() functions return a pointer so no mraa_result_t is available
        // however you can check for an allocation failure by checking the returned
        // pointer is not NULL. This likely doesn't mean that the malloc failed
        // internally but that the gpio requested is invalid. Cheking syslog
	// messages is the best way to have a look at what went wrong exactly.
	// Calling any further functions with a NULL pointer as a context is
	// undefined behaviour and will likely result in segfaults
	if (mygpio == NULL) {
            return MRAA_ERROR_INVALID_PARAMETER;
        }

	// GPIOs require you to set a direction before you can use them, there
	// is a default direction but this is very platform dependant so should
	// not be relied on
	result = mraa_gpio_dir(mygpio, MRAA_GPIO_OUT);
        if (result != MRAA_SUCCESS) {
            return result;
        }

        // Writing to a GPIO is very simple, simple write 1 (high) or 0 (low)
        result = mraa_gpio_write(mygpio, 1);
        if (result != MRAA_SUCCESS) {
            return result;
        }
    }

    // mraa_deinit() is really only useful for debugging/memory checking or if
    // mraa is loaded at runtime and not linked against. Any further mraa_
    // calls are expected to fail after this
    mraa_deinit();

    return result;
}
