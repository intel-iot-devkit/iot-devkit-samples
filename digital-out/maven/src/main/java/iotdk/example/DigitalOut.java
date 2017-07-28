/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 *
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
 * @brief Digital Output
 *
 * Demonstrate how to write a digital value to an output pin using the MRAA
 * library.
 * A suitable part to use this example with in the Grove Starter Kit is the LED.
 * TODO Use a platform with GPIO capabilities
 */
package iotdk.example;

import mraa.Dir;
import mraa.Gpio;
import mraa.mraa;
import mraa.Platform;
import mraa.Result;

public class DigitalOut {

    public static void main(String[] args) {
        // create a gpio object from MRAA using pin 8
        Gpio pin = new Gpio(8);

        // set the pin as output
        if (pin.dir(Dir.DIR_OUT) != Result.SUCCESS) {
            System.err.println("Can't set digital pin as output, exiting");
            return;
        }

        // loop forever toggling the digital output every second
        while (true) {
            pin.write(0);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
            pin.write(1);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
    }
}
