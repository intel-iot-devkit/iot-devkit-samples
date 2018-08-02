/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
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
 * Demonstrate how to react on an external event with an ISR (Interrupt Service
 * Routine), which will run independently of the main program flow using the
 * MRAA library.
 * Any button or sensor that can generate a voltage transition from ground to
 * Vcc or viceversa can be used with this example code.
 * Suitable ones in the Grove Starter Kit are the Button and Touch Sensor.
 *
 * Use a platform with GPIO Interrupt capabilities
 */
package iotdk.example;

import mraa.Dir;
import mraa.Edge;
import mraa.Gpio;
import mraa.mraa;
import mraa.Platform;
import mraa.Result;

class IsrCounterCallback implements Runnable {
    // counter that will be updated by the interrupt routine
    int counter = 0;

    public int getCounter() {
        return counter;
    }

    // ISR, update the value of the counter
    public void run() {
        counter++;
    }
}

public class Interrupt {

    static int pinNumber = 13;

    public static void checkRoot(){
        String username = System.getProperty("user.name");
        System.out.println(username);
        String message = "This project uses Mraa I/O operations, but you're not running as 'root'.\n"+
                "The IO operations below might fail.\nSee the project's Readme for more info.\n\n";
        if(!username.equals("root"))
        {
            System.out.println(message);
        }
    }

    public static void initPlatform(){
        Platform platform = mraa.getPlatformType();
        if(platform.equals(Platform.INTEL_MINNOWBOARD_MAX))
            pinNumber = 26;
        if(platform.equals(Platform.INTEL_JOULE_EXPANSION))
            pinNumber = 26;
        if(platform.equals(Platform.UNKNOWN_PLATFORM))
        {
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

        // set the pin as input
        if (pin.dir(Dir.DIR_IN) != Result.SUCCESS) {
            System.err.println("Can't set digital pin as input, exiting");
            return;
        }

        // set the ISR, it will be executed on both edges (on Galileo Gen 1 only
        // this mode is supported)
        IsrCounterCallback callback = new IsrCounterCallback();
        if (pin.isr(Edge.EDGE_BOTH, callback) != Result.SUCCESS) {
            System.err.println("Can't assign ISR to pin, exiting");
            return;
        }

        // loop forever printing the counter value every second
        while (true) {
            System.out.println(String.format("Counter value %d", callback.getCounter()));
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
    }
}
