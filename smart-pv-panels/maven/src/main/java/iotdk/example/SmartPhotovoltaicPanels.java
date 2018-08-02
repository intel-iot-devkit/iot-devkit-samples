/*
 * Author:
 * - Christian Roschat <christian.roschat@intel.com>
 * - Vittoriano Muttillo <vittoriano.muttillo@gmail.com>
 * - Giacomo Valente <giakomo.87v@gmail.com>
 *
 *
 * Copyright (c) 2016 Vittoriano Muttillo, Giacomo Valente.
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
 * Move photovoltaic panel following the maximum brightness of the sun.\n\n
 * The idea is to move photovoltaic panel (represented by a motor) according to
 * the movement of the sun, directing it towards the maximum light radiated
 * during the day using the maximum light intensity detected by two light
 * sensor placed parallel to each other.\n
 * The idea is to compute the sensors average light in a calibration step and
 * use this number to move motor according to the movement of light.
 *
 * @hardware Sensors used:\n
 * GroveLight sensor connected to the Grove Base Shield Port A0\n
 * GroveLight sensor connected to the Grove Base Shield Port A1\n
 * ULN200XA Stepper Motor connected to pin 6,7,8,9\n
 * Jhd1313m1 LCD connected to any I2C on the Grove Base Shield\n
 * Stepper Motor Driver Uln200xa connected in this way:\n
 *   -- I1  ->  pin D6\n
 *   -- I2  ->  pin D7\n
 *   -- I3  ->  pin D8\n
 *   -- I4  ->  pin D9\n
 *   -- GND ->  GND\n
 *   -- Vcc ->  5V (Vcc)\n
 *   -- Vm  ->  NC (Not Connected)
 *
 * Use a platform with I2C, Analog and GPIO capabilities
 */
package iotdk.example;

import mraa.Platform;
import mraa.mraa;
import upm_grove.GroveLight;
import upm_i2clcd.Jhd1313m1;
import upm_uln200xa.ULN200XA;
import upm_uln200xa.ULN200XA_DIRECTION_T;

public class SmartPhotovoltaicPanels {
    // Status of the correct r/w operation
    static final int SUCCESS = 0;

    static final boolean USING_GROVE_PI_SHIELD = true;

    static int aPin1 = 1;
    static int aPin2 = 2;
    static int i2cPort = 0;

    // Threshold of light
    static final int THRESHOLD = 2;

    // Steps per revolution
    static final int STEPS_PER_REV = 4096;

    // Left and right light average static variables
    static int lightLAVG, lightRAVG;

    static void solarTracker(Jhd1313m1 lcd, GroveLight leftLightSensor,
            GroveLight rightLightSensor, ULN200XA stepperMotor) {

        Integer lightLST, lightRST;

        // Since LCD displays data in string format, we need to convert (light value)
        // from integer to string
        lightLST = leftLightSensor.value();
        lightRST = rightLightSensor.value();

        // Writing light info on LCD
        lcd.setCursor(0, 0);
        lcd.write("Left:          ");
        lcd.setCursor(1, 0);
        lcd.write("Right:          ");
        lcd.setCursor(0, 6);
        if (lcd.write(lightLST.toString()) != SUCCESS)
            System.err.println("MRAA cannot display left value!");
        lcd.setCursor(1, 7);
        if (lcd.write(lightRST.toString())!= SUCCESS)
            System.err.println("MRAA cannot display right value!");

        /* To move the motor correctly, we have to choose the right direction.
         * To obtain it, we have three condition:
         * 1) The light sensors value is under a specific threshold (don't move)
         * 2) The Left light sensor value is under the average Left sensor value.
         *    In this case we have two condition:
         *    a) Left light sensor value is smaller than Right value (turn clockwise)
         *    b) Left light sensor value is bigger than Right value (turn counter clockwise)
         * 3) The Right light sensor value is under the average Right sensor value.
         *    In this case we have two condition:
         *    a) Right light sensor value is smaller than Left value (turn clockwise)
         *    b) Right light sensor value is bigger than Left value (turn counter clockwise)
         */

        // 1)
        if ((lightLST < THRESHOLD) && (lightRST < THRESHOLD)) {

            // Lightless state
            lcd.setCursor(0, 0);
            lcd.clear();
            lcd.write("No sun");

            // 2)
        } else if (lightLST < lightLAVG) {

            // a)
            if (lightLST < lightRST) {
                // Rotating 1/32 revolution clockwise
                stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CW);
                // b)
            } else if (lightLST > lightRST) {
                // Rotating 1/32 revolution clockwise
                stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CCW);
            }

            stepperMotor.stepperSteps(STEPS_PER_REV / 32);

            // 3)
        } else if (lightRST < lightRAVG) {

            // a)
            if (lightRST < lightLST) {
                // Rotating 1/32 revolution counter clockwise
                stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CCW);
                // b)
            } else if (lightRST > lightLST) {
                // Rotating 1/32 revolution counter clockwise
                stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CW);
            }
            stepperMotor.stepperSteps(STEPS_PER_REV / 32);
        }
    }

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

        if(platform.equals(Platform.INTEL_UP2)){
            if(USING_GROVE_PI_SHIELD) {
                mraa.addSubplatform(Platform.GROVEPI, "0");
                aPin1 = aPin1 + 512; 
                aPin2 = aPin2 + 512; 
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

        // LCD screen object (the lcd is connected to I2C port, bus 0)
        Jhd1313m1 lcd = new Jhd1313m1(i2cPort);

        // Left light sensor object (Analog pin 0)
        GroveLight lightL = new GroveLight(aPin1);

        // Right light sensor object (Analog pin 1)
        GroveLight lightR = new GroveLight(aPin2);

        /* Instantiate a Stepper motor on a ULN200XA Dual H-Bridge.
         * Wire the pins so that I1 is pin D6, I2 is D7, I3 is D8 and I4 is D9
         */
        ULN200XA stepperMotor = new ULN200XA(STEPS_PER_REV, 6, 7, 8, 9);

        // Simple error checking
        if ((lcd == null) || (lightL == null) || (lightR == null)
                || (stepperMotor == null)) {
            System.err.println("Could not create all objects, exiting");
            return;
        }

        // Writing intro text
        lcd.setCursor(0, 0);
        lcd.write("Smart PV");
        lcd.setCursor(1, 0);
        lcd.write("for Edison Board");

        // Set speed of Grove Gear Stepper Motor with Drive
        stepperMotor.setSpeed(7);

        // Calibration Step
        int brightL1, brightL2, brightR1, brightR2;

        // Rotating 1/8 revolution clockwise
        stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CW);
        stepperMotor.stepperSteps(STEPS_PER_REV / 8);
        brightL1 = lightL.value();
        brightR1 = lightR.value();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            System.err.println("Sleep interrupted: " + e.toString());
        }

        // Rotating 1/4 revolution counter clockwise
        stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CCW);
        stepperMotor.stepperSteps(STEPS_PER_REV / 4);
        brightL2 = lightL.value();
        brightR2 = lightR.value();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            System.err.println("Sleep interrupted: " + e.toString());
        }

        // Compute average light sensor value
        lightLAVG = (brightL1 + brightL2) / 2;
        lightRAVG = (brightR1 + brightR2) / 2;

        // Init text on display

        // Loop forever updating the lightL and lightR values every second
        while (true) {
            solarTracker(lcd, lightL, lightR, stepperMotor);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }
    }
}
