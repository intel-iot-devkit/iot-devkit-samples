/*
 * Author:
 * - Christian Roschat <christian.roschat@intel.com>
 * - Jessica Gomez <jessica.gomez.hernandez@intel.com>
 *
 * Copyright (c) 2016 Intel Corporation.
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
 * Demonstrate how to use PWM with an output pin using the MRAA library.
 * If the output is connected to a led, its intensity, as perceived by the human
 * eye, will vary depending on the duty cycle.
 * A suitable part to use this example with in the Grove Starter Kit is the LED.
 *
 * Use a platform with PWM capabilities
 */
package iotdk.example;

import mraa.Pwm;
import mraa.mraa;
import mraa.Platform;
import mraa.Result;

public class PWM {
    // Set true if using a Grove Pi Shield, else false
    static final boolean USING_GROVE_PI_SHIELD = true;

    static int pinNumber = 33;

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
                pinNumber = pinNumber + 512; // A5 Connector (512 offset needed for the shield)
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

        // create a PWM object from MRAA using pin 5
        // note that not all digital pins can be used for PWM, the available ones
        // are usually marked with a ~ on the board's silk screen
        Pwm pwm_pin = null;
        pwm_pin = new Pwm(pinNumber);
        // select PWM period of 1ms
        if (pwm_pin.period_ms(1) != Result.SUCCESS) {
            System.err.println("Could not initalize the PMW period, exiting");
            return;
        }

        // enable the pin for ouput
        if (pwm_pin.enable(true) != Result.SUCCESS) {
            System.err.println("Could not enable the pin for output, exiting");
            return;
        };

        // PWM duty-cycle value,  0 == 0%, 1 == 100%
        float duty_cycle = 0;

        // loop forever increasing the PWM duty-cycle from 0% to 100%
        while (true) {
            // increase the duty-cycle by 1%
            duty_cycle += 0.01;
            // set the output duty-cycle percentage
            if (pwm_pin.write(duty_cycle) != Result.SUCCESS) {
                System.err.println("Could not set the duty-cycle percentage, exiting");
                return;
            };

            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
            // if the duty-cycle reaches 100% start from 0% again
            // a full cycle will take ~5 seconds
            if (duty_cycle >= 1) {
                duty_cycle = 0;
            }
        }
    }
}
