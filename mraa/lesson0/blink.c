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
#include <unistd.h>

// libmraa is the library used to interface with the IO on iot-devkit
// platforms. You use it by including the top level mraa.h header in
// /usr/include and linking your application against -lmraa.

// Typically the compilation line would look something like:
// $ gcc myprog.c -lmraa
#include "mraa.h"

void
main(int argc, char **argv)
{
    // mraa gives you control of a gpio via a handle or context, we'll request
    // a context to represent io13 on our board
    mraa_gpio_context mygpio = mraa_gpio_init(13);
    // GPIOs require you to set a direction before you can use them
    mraa_gpio_dir(mygpio, MRAA_GPIO_OUT);
    // We'll store the value we want to write to our gpio here
    int val = 0;
    for (;;) {
        // flip our value
        val = !val;
        // write the value to our gpio
        mraa_gpio_write(val);
        // sleep(3) uses seconds
        sleep(1);
    }
}
