/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
 *
 * Demonstrate how to read an analog voltage value from an input pin using the
 * MRAA library, any sensor that outputs a variable voltage can be used with
 * this example code.
 * Suitable ones in the Grove Starter Kit are the Rotary Angle Sensor, Light
 * Sensor, Sound Sensor, Temperature Sensor.
 *
 * Use platform with Analog capabilities.
 */
package iotdk.example;

import mraa.Aio;
import mraa.Platform;
import mraa.mraa;

public class AnalogIn {

    public static void main(String[] args) {

        // create an analog input object from MRAA using pin A0
        Aio pin = new Aio(0);

        // loop forever printing the input value every second
        while (true) {
            long value = pin.read();
            System.out.println(String.format("Pin value: %d", value));
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
    }
}
