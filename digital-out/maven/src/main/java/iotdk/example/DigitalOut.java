/*
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
 * Use a platform with GPIO capabilities
 */
package iotdk.example;

import mraa.Dir;
import mraa.Gpio;
import mraa.mraa;
import mraa.Platform;
import mraa.Result;

public class DigitalOut {

    // Set true if using a Grove Pi Shield, else false
    static final boolean USING_GROVE_PI_SHIELD = true;
    static int pinNumber = 13;

    public static void checkRoot(){
        String username = System.getProperty("user.name");

        String message = "This project uses Mraa I/O operations, but you're not running as 'root'.\n"+
                "The IO operations below might fail.\nSee the project's Readme for more info.\n\n";
        if(!username.equals("root"))
        {
            System.out.println(message);
        }
    }

    public static void initPlatform(){
        Platform platform = mraa.getPlatformType();
        if(platform.equals(Platform.INTEL_UP2)) {
            if(USING_GROVE_PI_SHIELD) {
                mraa.addSubplatform(Platform.GROVEPI, "0");
                pinNumber = 4 + 512; // D4 Connector (512 offset needed for the shield)
            }
        } else {
            String unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, " +
                    "you are running it on an unrecognized platform. " +
                    "You may need to modify the MRAA/UPM initialization code to " +
                    "ensure it works properly on your platform.\n\n";
            System.err.println(unknownPlatformMessage);
        }
    }


    public static void main(String[] args) {

        checkRoot();
        initPlatform();

        // create a gpio object from MRAA
        Gpio pin = new Gpio(pinNumber);

        // set the pin as output
        if (pin.dir(Dir.DIR_OUT) != Result.SUCCESS) {
            System.err.println("Can't set digital pin as output, exiting");
            return;
        }

        // loop forever toggling the digital output every second
        while (true) {
            if (pin.write(0) != Result.SUCCESS)
                System.err.println("MRAA cannot write pin value!");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
            if (pin.write(1) != Result.SUCCESS)
                System.err.println("MRAA cannot write pin value!");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
    }
}
