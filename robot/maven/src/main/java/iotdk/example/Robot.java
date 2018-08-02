/*
 * Author: Mihai T Panu <mihai.tudor.panu@intel.com>
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
 * @ingroup other
 * @brief Robot
 *
 * This project template utilizes components in the Grove Robotics Kit to create a working robot.
 * Known Limitation: This demo needs to be ran directly over a ssh or serial connection for full functionality,
 * Eclipse does not forward console input.
 *
 * @hardware - Grove IR Distance Interrupter (SEN09281P)
 * - Grove LCD RGB Backlight (811004001)
 * - Grove 3-Axis Digital Compass (SEN12753P)
 * - Grove I2C Motor Driver (ROB72212P)
 * - 4x Seeed 25GA35 DC Motors (RK-370C-3080EP)
 * Additional parts:
 * - SeeedStudio 4WD Hercules
 * - Mobile Platform Kit
 * - 7.4 V (2-cell) LiPo Battery
 *
 *  Use a platform with I2C and GPIO capabilities
 */
package iotdk.example;

import java.util.Scanner;

import upm_grovemd.GroveMD;
import upm_grovevdiv.GroveVDiv;
import upm_hmc5883l.Hmc5883l;
import upm_i2clcd.Jhd1313m1;
import upm_rfr359f.RFR359F;

public class Robot {

    static boolean running = true;

    static boolean batteryLow = true;
    static float batteryThreshold = 7.2f;

    final static String heading = "--|--N--|--|--E--|--|--S--|--|--W--|--|--N--|--";

    static boolean blockedFL = false;
    static boolean blockedFR = false;
    static boolean blockedRL = false;
    static boolean blockedRR = false;

    final static int EXIT_FAILURE = 1;

    static GroveMD motors;
    static Jhd1313m1 lcd;
    static Hmc5883l compass;
    static GroveVDiv divider;

    static RFR359F frontLeftIR;
    static RFR359F frontRightIR;
    static RFR359F rearLeftIR;
    static RFR359F rearRightIR;
    
    // Status of the correct r/w operation
    static final int SUCCESS = 0;

    static Runnable shutdown = new Runnable() {

        @Override
        public void run() {
            running = false;
        }
    };

    static Runnable compRunnable = new Runnable() {

        @Override
        public void run() {

            while(running){
                compass.update();

                // Round the heading returned by compass and turn it into an index
                // from 0 to 36 for the heading string
                int hdg = (int)(compass.heading() + 0.5f);
                int hdg_index = hdg / 10;
                if (hdg_index < 0)
                    hdg_index = 0;
                if (hdg_index > 36)
                    hdg_index = 36;
                // Write the heading on the first line, lcd.clear() is not needed since
                // we write the entire line
                synchronized (lcd) {
                    lcd.setCursor(0, 0);
                    if (lcd.write("HDG: " + heading.substring(hdg_index, hdg_index + 11)) != SUCCESS)
                        System.err.println("MRAA cannot display heading!");
                }

                // Update readings and display every 250 ms
                try {
                    Thread.sleep(250);
                } catch (InterruptedException e) {
                    System.err.println("Sleep interrupted: " + e.toString());
                }

            }
        }
    };

    static Runnable battRunnable = new Runnable() {

        @Override
        public void run() {


            // Variable used for flashing LCD red when battery is low
            short red = 0x3F;

            while(running){
                // Read 50 samples each with 2 ms in between (100 ms total)
                int avgValue = (int) divider.value(50);
                // Convert the value to voltage at 3x gain and 5V reference
                float voltage = divider.computedValue((short)3, avgValue);
                String displayStr = Float.toString(voltage);
                displayStr = displayStr.substring(0, 4);

                // Write the battery voltage on the second line of the display
                synchronized (lcd) {
                    lcd.setCursor(1, 0);
                    if (lcd.write("Batt: " + displayStr + " V    ") != SUCCESS)
                        System.err.println("MRAA cannot display voltage!");    
                }

                // Battery low, flash LCD and refresh more often
                if(voltage < batteryThreshold)
                {
                    batteryLow = true;
                    lcd.setColor(red, (short)0x00, (short)0x00);
                    // Toggle red bits
                    red = (short) ~red;
                    try {
                        Thread.sleep(2000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                }
                else{
                    // Battery was considered low but new reading is above threshold
                    // Thus return to normal operation mode and make the LCD green
                    if(batteryLow){
                        lcd.setColor((short)0x00, (short)0xCF, (short)0x00);
                    }
                    batteryLow = false;
                    // Refresh every 5 seconds
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                        System.err.println("Sleep interrupted: " + e.toString());
                    }
                }
            }
        }
    };

    static Runnable collisionRunnable = new Runnable() {

        @Override
        public void run() {

            while(running){
                // Set the corresponding sensor variable when an object is detected
                blockedFL = frontLeftIR.objectDetected();
                blockedFR = frontRightIR.objectDetected();
                blockedRL = rearLeftIR.objectDetected();
                blockedRR = rearRightIR.objectDetected();

                // Stop the motors if any sensor was triggered
                if(blockedFL || blockedFR || blockedRL || blockedRR){
                    System.out.println("Sensors blocked. Robot is stopping.");
                    motors.setMotorSpeeds((short)0, (short)0);
                }
                // Refresh every 10 ms
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    System.err.println("Sleep interrupted: " + e.toString());
                }
            }
        }
    };

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

    public static void main(String[] args) {

        checkRoot();

        // Register signal handler
        Runtime runtime = Runtime.getRuntime();
        runtime.addShutdownHook(new Thread(shutdown));

        // Instantiate an I2C Grove Motor Driver on default I2C bus
        // This will also set the I2C bus to 100 kHz for compatibility
        // Default address of 0x0f is used (all 4 switches in up position)
        motors = new GroveMD(0, (short)0xf);

        // Initialize the Grove RGB Backlit LCD using implicit
        // 0x62 for RGB_ADDRESS and 0x3E for LCD_ADDRESS
        lcd = new Jhd1313m1(0);

        // Instantiate the HMC5883 compass on default I2C bus
        // The compass requires an I2C level translator to 3.3V if used
        compass = new Hmc5883l(0);

        // Instantiate the Grove Voltage Divider on pin A0
        divider = new GroveVDiv(0);

        // Instantiate 4 Grove IR Distance Interrupters on pins D2, D4, D6, D8
        frontLeftIR = new RFR359F(2);
        frontRightIR = new RFR359F(4);
        rearLeftIR = new RFR359F(6);
        rearRightIR = new RFR359F(8);

        Scanner s = new Scanner(System.in);

        // Start independent threads for the different components
        // If you're not using a component comment out the corresponding thread call
        //thread collision (distanceIR, motors, frontLeftIR, frontRightIR, rearLeftIR, rearRightIR);
        Thread compThread = new Thread(compRunnable);
        compThread.start();
        Thread battThread = new Thread(battRunnable);
        battThread.start();
        Thread collisionThread = new Thread(collisionRunnable);
        collisionThread.start();

        // Main event and control loop
        // Commands are given through stdin as tuples of the form <CMD_NAME SPEED>
        // Except for the <stop> command and are case sensitive
        while(running){

            if (!s.hasNextLine()) {
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    System.err.println("Sleep interrupted: " + e.toString());
                }
                continue;
            }

            String command = s.next();
            short speed = 0;

            if(command == "stop" || command == ""){
                // Stop rover
                motors.setMotorSpeeds((short)0, (short)0);
                System.out.println("Rover stopping!");
                continue;
            }
            // Read speed
            speed = s.nextShort();

            // Check for proper range
            if(speed < 0 || speed > 255){
                System.err.println("Error: Speed needs to be between 0 to 255.");
                continue;
            }

            // Direction is set based on command/keyword
            if(command.equals("fwd") && (!blockedFL || !blockedFR)){
                motors.setMotorDirections(GroveMD.DC_DIRECTION_T.DIR_CCW,
                        GroveMD.DC_DIRECTION_T.DIR_CCW);
                motors.setMotorSpeeds(speed, speed);
                System.out.println("Rover going forward at speed " + speed);
            }
            else if(command.equals("left")  && (!blockedFL || !blockedRL)){
                motors.setMotorDirections(GroveMD.DC_DIRECTION_T.DIR_CCW, GroveMD.DC_DIRECTION_T.DIR_CCW);
                motors.setMotorSpeeds(speed, speed);
                System.out.println("Rover turning left at speed " + speed);
            }
            else if(command.equals("right") && (!blockedFR || !blockedRR)){
                motors.setMotorDirections(GroveMD.DC_DIRECTION_T.DIR_CW, GroveMD.DC_DIRECTION_T.DIR_CW);
                motors.setMotorSpeeds(speed, speed);
                System.out.println("Rover turning right at speed " + speed);
            }
            else if(command.equals("rev")  && (!blockedRL || !blockedRR)){
                motors.setMotorDirections(GroveMD.DC_DIRECTION_T.DIR_CW, GroveMD.DC_DIRECTION_T.DIR_CCW);
                motors.setMotorSpeeds(speed, speed);
                System.out.println("Rover in reverse at speed " + speed);
            }
            else{
                motors.setMotorSpeeds((short)0, (short)0);
                System.out.println("Command not supported or direction blocked!");
            }
        }

        // Wait for threads to join back
        // If you're not using a component comment out the corresponding join call
        try {
            compThread.join();
            battThread.join();
            collisionThread.join();
        } catch (InterruptedException e) {
            System.err.println("Sleep interrupted: " + e.toString());
        }

        s.close();
        System.out.println("Exiting...");
    }

}
