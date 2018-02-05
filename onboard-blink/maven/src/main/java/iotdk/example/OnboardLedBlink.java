/*
 * Author: Jessica Gomez <jessica.gomez.hernandez@intel.com>
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
 * Demonstrate how to blink the on board LED, writing a digital value to an
 * output pin using the MRAA library.
 *
 * use a platform with GPIO capabilities and an onboard LED
 */
package iotdk.example;

import mraa.Dir;
import mraa.Gpio;
import mraa.Platform;
import mraa.Result;
import mraa.mraa;

public class OnboardLedBlink {

    public static void main(String[] args) {
        // Change the GPIO to one that matches your platform
        // Intel Galileo Gen 2 = 13
        // Intel Edison = 13
        // Intel Joule with expansion board = 100
        // set the pin as output
        Gpio pin = new Gpio(13);

        if (pin == null) {
            System.err.println("Could not initialize GPIO, exiting");
            return;
        }

        if (pin.dir(Dir.DIR_OUT) != Result.SUCCESS) {
            System.err.println("Could not set pin as output, exiting");
            return;
        }

        // loop forever toggling the on board LED every second
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
