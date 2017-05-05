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
 * @brief Digital Input
 *
 * Demonstrate how to read a digital value from an input pin using the MRAA
 * library.
 * Suitable ones in the Grove Starter Kit are the Button and Touch Sensor.
 *
 * @hardware Input connected to digital pin 4 (Grove Base Shield Port D4)
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

public class DigitalIn {

    public static void main(String[] args) {
        // check that we are running on Galileo or Edison
        Platform platform = mraa.getPlatformType();
        if (platform != Platform.INTEL_GALILEO_GEN1 &&
                platform != Platform.INTEL_GALILEO_GEN2 &&
                platform != Platform.INTEL_EDISON_FAB_C &&
                platform != Platform.MRAA_INTEL_JOULE_EXPANSION) {
            System.err.println("Unsupported platform, exiting");
            return;
        }

        // create a GPIO object from MRAA using pin 4
        Gpio pin = new Gpio(4);

        // set the pin as input
        if (pin.dir(Dir.DIR_IN) != Result.SUCCESS) {
            System.err.println("Can't set digital pin as input, exiting");
            return;
        }

        // loop forever printing the digital input value every second
        while (true) {
            int value = pin.read();
            System.out.println(String.format("Pin value: %d", value));
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
    }
}