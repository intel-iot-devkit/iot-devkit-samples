/*
 * Authors:
 * - Bufalino Andrea <andrea.bufalino@outlook.com>
 * - Carocci Eugenio <carocci.eugenio@gmail.com>
 * - Ranalli Alessandro <ranalli.alessandro1@gmail.com>
 * - Christian Roschat <christian.roschat@intel.com>
 *
 * Copyright (c) 2016 Bufalino Andrea, Carocci Eugenio, Ranalli Alessandro.
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
 * @ingroup other
 * @brief Automatic curtain
 *
 * Open and close automatically a curtain according to the luminosity\n
 * Warning: a 5V power supply is required in order to run this example\n
 * \n
 * First set the wished luminosity with the Grove Rotary Angle Sensor. Then
 * push the Grove Button and the program will try to reach this luminosity by
 * opening/closing the curtain.
 *
 * @hardware Sensors used:\n
 * Grove Rotary Angle Sensor connected to the Grove Base Shield Port A0\n
 * Grove Light Sensor connected to the Grove Base Shield Port A3\n
 * Grove Button connected to the Grove Base Shield Port D4\n
 * Jhd1313m1 LCD connected to any I2C on the Grove Base Shield\n
 * Stepper Motor Driver Uln200xa connected in this way:\n
 *   -- I1  ->  pin D8\n
 *   -- I2  ->  pin D9\n
 *   -- I3  ->  pin D10\n
 *   -- I4  ->  pin D11\n
 *   -- GND ->  GND\n
 *   -- Vcc ->  5V (Vcc)\n
 *   -- Vm  ->  NC (Not Connected)
 *
 * @req mraa.jar
 * @req upm_grove.jar
 * @req upm_i2clcd.jar
 * @req upm_uln200xa.jar
 *
 * @date 16/06/2016
 */

/*
 *  Hypothesis:
 * - curtain initially open;
 * - to close completely the courtain 2 stepper motor's full revolutions are
 *   needed;
 * - lux range in [0-60].
 *
 *   Use a platform with Analog Input, I2C and GPIO capabilities
 */
package iotdk.example;

import upm_grove.GroveButton;
import upm_grove.GroveLight;
import upm_grove.GroveRotary;
import upm_i2clcd.Jhd1313m1;
import upm_uln200xa.ULN200XA;
import upm_uln200xa.ULN200XA_DIRECTION_T;
import mraa.mraa;
import mraa.Platform;


public class AutomaticCurtain {
    static final boolean USING_GROVE_PI_SHIELD = true;
    // Speed of the stepper motor in revolutions per minute (RPM).
    static final int STEPPER_MOTOR_SPEED = 5;
    // Number of steps per full revolution.
    static final int STEPPER_MOTOR_STEPS_FULL_REVOLUTION = 4096;
    // Steps required to draw the curtain completely.
    static final int STEPPER_MOTOR_MAX_STEPS = 2 * STEPPER_MOTOR_STEPS_FULL_REVOLUTION;
    /*
     * Number of steps made by the stepper motor for each activation.
     * To complete a whole revolution 4096 steps are required, so using 1024
     * steps, for each activation 1/4 revolution is made.
     */
    static final int STEPPER_MOTOR_ACTIVATION_STEPS = 1024;

    /* Lux threshold. Small light fluctuations can cause continuous activity
     * of the stepper motor.
     * To prevent this behavior a small hysteresis of few lux units is
     * integrated.
     */
    static final int THRESHOLD = 5;

    // Status of the correct r/w operation
    static final int SUCCESS = 0;

    // Position of stepper motor.
    static int stepper_motor_current_step = 0;

    static int aPin1 = 1; 
    static int aPin2 = 2; 
    static int dPin4 = 4;
    static int  i2cPort = 0;

    /*
     * System can be in two states:
     * - CONFIG: A lux target can be specified from the user.
     * - NORMAL: System try to mantain or reach the lux target value.
     */
    static enum State {
        CONFIG, NORMAL
    };

    /**
     * Get the desired lux target value from the user.
     *
     * @param rotary_sensor Rotary angle sensor.
     * @param light_sensor  Light sensor.
     * @param button    Button.
     * @param lcd       LCD.
     * @return Lux value setted by the user through rotary angle sensor.
     */
    static int setupLuxTarget(GroveRotary rotarySensor,
            GroveLight lightSensor, GroveButton button,
            Jhd1313m1 lcd) {
        int luxTarget = 0;
        boolean confirmed = false;

        while (!confirmed) {
            /*
             * Rotary_sensor->abs_deg() in within the interval [0-300].
             * Dividing Rotary_sensor->abs_deg() by 6 interval [0-60] is
             * obtained.
             */
            luxTarget = (int) (rotarySensor.abs_deg() / 5);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.write("Btn to confirm");
            lcd.setCursor(1, 0);
            lcd.cursorBlinkOn();
            lcd.write("Lux Target: " + luxTarget);
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
            // The button has been pressed
            if (button.value() == 1) {
                confirmed = true;
                lcd.cursorBlinkOff();
            }
        }

        return luxTarget;
    }

    /**
     * Show on the LCD the current lux value sensed and the lux target value.
     *
     * @param lcd LCD.
     * @param lux_target Lux target value.
     * @param lux_current Lux current value.
     */
    static void showOnLcd(Jhd1313m1 lcd, int luxTarget, int luxCurrent) {
        String row1, row2; // LCD rows
        row1 = "Lux Current: " + luxCurrent;
        row2 = "Lux Target:  " + luxTarget;
        lcd.clear();
        lcd.setCursor(0, 0);
        if (lcd.write(row1) != SUCCESS)
            System.err.println("MRAA cannot write current lux value!");
        lcd.setCursor(1, 0);
        if (lcd.write(row2) != SUCCESS)
            System.err.println("MRAA cannot write target lux value!");
    }

    /**
     * Draw (partially) the curtain, if it is not already completely closed.
     *
     * @param stepperMotor Stepper motor driver.
     * @return True if the curtain has been partially closed. False if the curtain
     *     is already completely closed.
     */
    static boolean drawCurtain(ULN200XA stepperMotor) {
        // Check if the curtain is already completely closed.
        if (stepper_motor_current_step != STEPPER_MOTOR_MAX_STEPS) {
            stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CW);
            stepperMotor.stepperSteps(STEPPER_MOTOR_ACTIVATION_STEPS);
            stepper_motor_current_step += STEPPER_MOTOR_ACTIVATION_STEPS;
            stepperMotor.release();
            return true;
        } else {
            // Curtain already completely closed
            return false;
        }
    }

    /**
     * Open (partially) the curtain, if it is not already completely open.
     *
     * @param stepperMotor Stepper motor driver.
     * @return  True if the curtain has been partially opened. False if the
     * curtain is already completely open.
     */
    static boolean openCurtain(ULN200XA stepperMotor) {
        if (stepper_motor_current_step != 0) {
            stepperMotor.setDirection(ULN200XA_DIRECTION_T.ULN200XA_DIR_CCW);
            stepperMotor.stepperSteps(STEPPER_MOTOR_ACTIVATION_STEPS);
            stepper_motor_current_step -= STEPPER_MOTOR_ACTIVATION_STEPS;
            stepperMotor.release();
            return true;
        } else {
            // Curtain already completely open.
            return false;
        }
    }

    /**
     * Check the current lux value sensed by the light sensor, and if
     * it is needed, open or draw the curtain.
     *
     * @param lightSensor Light sensor.
     * @param lcd          LCD.
     * @param stepperMotor Stepper motor driver.
     * @param luxTarget   Lux target value.
     */
    static void checkLux(GroveLight lightSensor, Jhd1313m1 lcd,
            ULN200XA stepperMotor, int luxTarget) {

        int luxCurrent;
        // Get current lux value sensed by the light sensor.
        luxCurrent = lightSensor.value();

        showOnLcd(lcd, luxTarget, luxCurrent);

        if (luxCurrent > luxTarget + THRESHOLD) {
            // Too much light, so draw the curtain.
            System.out.println("Too much light, so draw the curtain.");
            if (!drawCurtain(stepperMotor)) {
                System.out.println("Curtain already completely closed");
            }
        } else if (luxCurrent < luxTarget - THRESHOLD) {
            // Too few light, so open the curtain.
            System.out.println("Too few light, so open the curtain");
            if (!openCurtain(stepperMotor)) {
                System.out.println("Curtain already completely open");
            }
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
                dPin4 = dPin4 + 512;
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

        // Starting state for the system
        State status = State.CONFIG;

        // Lux target value
        int luxTarget = 0;

        // Instantiate a rotary sensor on analog pin A1
        GroveRotary rotarySensor = new GroveRotary(aPin1);

        // Instantiate a light sensor on analog pin A2
        GroveLight lightSensor = new GroveLight(aPin2);

        // Instantiate a button on digital pin D4
        GroveButton button = new GroveButton(dPin4);

        /* Instantiate a stepper motor driver wiring the pins
         * so that I1 is pin D8, I2 is pin D9, I3 is pin D10 and
         * I4 is pin D11
         */
        ULN200XA stepperMotor = new ULN200XA(
                STEPPER_MOTOR_STEPS_FULL_REVOLUTION, 8, 9, 10, 11);

        // LCD connected to the default I2C bus
        Jhd1313m1 lcd = new Jhd1313m1(i2cPort);

        // Simple error checking
        if ((rotarySensor == null) || (lightSensor == null) || (button == null)
                || (stepperMotor == null) || (lcd == null)) {
            System.err.println("Could not create all objects, exiting");
            return;
        }

        stepperMotor.setSpeed(STEPPER_MOTOR_SPEED);

        // Loop forever
        for (;;) {

            // The button has been pressed
            if (button.value() == 1) {
                status = State.CONFIG;
            }

            if (status == State.NORMAL) {
                // NORMAL STATE
                checkLux(lightSensor, lcd, stepperMotor, luxTarget);
            } else {
                // CONFIG STATE
                luxTarget = setupLuxTarget(rotarySensor, lightSensor, button, lcd);
                status = State.NORMAL;
            }
            // Sleep for 500 ms
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }

        }
    }
}
