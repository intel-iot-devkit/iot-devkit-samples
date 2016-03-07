/*
 * Author: Sisinty Sasmita Patra <sisinty.s.patra@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 * 
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

/**
 * @file
 * @ingroup grove 
 * @brief Various Sensors
 *
 * Demonstrate how to read a digital value from an input pin using the MRAA
 * library.
 * Suitable ones in the Grove Starter Kit are the Button and Touch Sensor.
 * 
 * @hardware - Grove Temperature Sensor connected to Grove Base Shield Port A1
 * - JHD1313M1 LCD connected to any I2C port on the Grove Base Shield
 * - Grove Button connected to the Grove Base Shield Port D2
 * - Grove Rotary connected to the Grove Base Shield Port A0
 * - Grove Light connected to the Grove Base Shield Port A2
 * - Grove Touch Sensor connected to the Grove Base Shield Port D4
 * - MMA7660 Accelerometer connected to any I2C port on the Grove Base Shield
 * 
 * @req mraa.jar
 * @req upm_grove.jar
 * @req upm_i2clcd.jar
 * @req upm_ttp223.jar
 * @req upm_mma7660.jar
 *
 * @date 19/08/2015
 */

import upm_grove.GroveButton;
import upm_grove.GroveLight;
import upm_grove.GroveRotary;
import upm_grove.GroveTemp;
import upm_i2clcd.Jhd1313m1;
import upm_mma7660.MMA7660;
import upm_ttp223.TTP223;
import mraa.Platform;
import mraa.mraa;

public class VariousSensors {

    public static void main(String[] args) {
        // check that we are running on Galileo or Edison
        Platform platform = mraa.getPlatformType();
        if (platform != Platform.INTEL_GALILEO_GEN1 &&
                platform != Platform.INTEL_GALILEO_GEN2 &&
                platform != Platform.INTEL_EDISON_FAB_C) {
            System.err.println("Unsupported platform, exiting");
            return;
        }

        // Create the temperature sensor object using AIO pin 1
        GroveTemp temp = new GroveTemp(1);

        // Create the lcd sensor object using I2C pin
        Jhd1313m1 lcd = new Jhd1313m1(0, 0x3E, 0x62);

        // Create the button object using GPIO pin 2
        GroveButton button = new GroveButton(2);

        // Create the rotary Sensor object using AIO pin 0
        GroveRotary knob = new GroveRotary(0);

        // Create the light sensor object using AIO pin 2
        GroveLight light = new GroveLight(2);

        // Create the TTP223 touch sensor object using GPIO pin 4
        TTP223 touch = new TTP223(4);

        // Create the MMA7660 accelerometer on I2C bus 0
        MMA7660 accel = new MMA7660(0);

        // place device in standby mode so we can write registers
        accel.setModeStandby();

        // enable 64 samples per second
        accel.setSampleRate(MMA7660.MMA7660_AUTOSLEEP_T.AUTOSLEEP_64);

        // place device into active mode
        accel.setModeActive();

        int celsius;
        float rotary;
        int lit;
        boolean tuch;
        float acc[];

        // This for loop is required to get the updated value from respective sensors
        for (int i=0; i < 5; i++) {
            celsius = temp.value();
            rotary = knob.abs_deg();
            lit = light.value();
            tuch = touch.isPressed();
            acc = accel.getAcceleration();
        }

        lcd.setCursor(0,0);
        lcd.write("welcome to ");
        lcd.setCursor(1,0);
        lcd.write("Starter Kit!");

        try {
            Thread.sleep(3000);
        } catch (InterruptedException e) {
            System.err.println("Sleep interrupted: " + e.toString());
        }
        lcd.clear();

        /* This variable helps to display one particular sensor at any given
         * time, when button value is '1' */
        int x = 0;

        /* This while loop continously checks for button value.
         * if button value is '0', the LCD displays "press button".
         * if button value is '1', the sensors values are displayed depending
         * on x value
         */
        while(true)
        {
            if(button.value() == 0)
            {
                lcd.setCursor(0,0);
                lcd.write("Press Button ");
            }
            else
            {
                if(x == 0)
                {
                    celsius = temp.value();

                    // Since LCD displays data in string format,
                    // we need to convert (celsius value) from integer to string
                    String tdata = Integer.toString(celsius);

                    lcd.clear();
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }

                    lcd.setCursor(0,0);
                    lcd.write("Temperature in ");
                    lcd.setCursor(1,2);
                    lcd.write("celsius: ");
                    lcd.write(tdata);

                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.clear();
                    x += 1;
                }

                else if(x == 1)
                {
                    rotary = knob.abs_deg();
                    String rotdata = Float.toString(rotary);

                    lcd.clear();
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }

                    lcd.setCursor(0,0);
                    lcd.write("Rotatory Angle ");
                    lcd.setCursor(1,2);
                    lcd.write("in degree: ");
                    lcd.write(rotdata);

                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.clear();
                    x += 1;
                }

                else if(x == 2)
                {
                    lit = light.value();
                    String litdata = Integer.toString(lit);

                    lcd.clear();
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }

                    lcd.setCursor(0,0);
                    lcd.write("light value ");
                    lcd.setCursor(1,2);
                    lcd.write("in lux: ");
                    lcd.write(litdata);

                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.clear();
                    x += 1;
                }

                else if(x == 3)
                {
                    tuch = touch.isPressed();
                    String touchdata = Boolean.toString(tuch);

                    lcd.clear();
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }

                    lcd.setCursor(0,0);
                    lcd.write("Touch Sensor ");
                    lcd.setCursor(1,2);
                    lcd.write("is pressed: ");
                    lcd.write(touchdata);

                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.clear();
                    x += 1;
                }

                else if(x == 4)
                {
                    acc = accel.getAcceleration();

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.setCursor(0,0);
                    lcd.write("Acceleration x: ");
                    lcd.setCursor(1,2);
                    lcd.write(Float.toString(acc[0]));
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }

                    lcd.clear();

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.setCursor(0,0);
                    lcd.write("Acceleration y: ");
                    lcd.setCursor(1,2);
                    lcd.write(Float.toString(acc[1]));
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }

                    lcd.clear();

                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.setCursor(0,0);
                    lcd.write("Acceleration z: ");
                    lcd.setCursor(1,2);
                    lcd.write(Float.toString(acc[2]));

                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                    lcd.clear();
                    x = 0;
                }
            }
        }

    }

}
