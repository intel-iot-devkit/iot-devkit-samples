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
 * @file
 * @ingroup basic
 * @brief On board LED blink Java
 *
 * Demonstrate how to blink the on board LED, writing a digital value to an
 * output pin using the MRAA library.
 *
 * @hardware No external hardware is needed.
 *
 * @req mraa.jar
 *
 * @date 19/08/2015
 */
package iotdk.example;

import mraa.Dir;
import mraa.Gpio;
import mraa.Platform;
import mraa.Result;
import mraa.mraa;

public class OnboardLedBlink {

    public static void main(String[] args) {
        // select onboard LED pin based on the platform type
        // create a GPIO object from MRAA using it
        Platform platform = mraa.getPlatformType();
        Gpio pin = null;
        if (platform == Platform.INTEL_GALILEO_GEN1)
            pin = new Gpio(3);
        else if (platform == Platform.INTEL_GALILEO_GEN2)
            pin = new Gpio(13);
        else if (platform == Platform.INTEL_EDISON_FAB_C)
            pin = new Gpio(13);
        else if (platform == Platform.MRAA_INTEL_JOULE_EXPANSION)
            pin = new Gpio(100);

        // set the pin as output
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