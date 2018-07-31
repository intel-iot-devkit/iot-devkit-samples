/*
 * Author: Mihai T Panu <mihai.tudor.panu@intel.com>
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
 * This project template utilizes components in the Grove Robotics Kit to create a working robot.
 *
 * Hardware - Grove IR Distance Interrupter (SEN09281P)
 * - Grove LCD RGB Backlight (811004001)
 * - Grove 3-Axis Digital Compass (SEN12753P)
 * - Grove I2C Motor Driver (ROB72212P)
 * - 4x Seeed 25GA35 DC Motors (RK-370C-3080EP)
 * Additional parts:
 * - SeeedStudio 4WD Hercules
 * - Mobile Platform Kit
 * - 7.4 V (2-cell) LiPo Battery
 *
 * Use a platform with I2C, GPIO capabilities
 */

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include "grovemd.hpp"
#include "jhd1313m1.hpp"
#include "rfr359f.hpp"
#include "hmc5883l.hpp"
#include "grovevdiv.hpp"

using namespace std;
using namespace upm;

mutex crit;
bool running = true;

bool blockedFL = false;
bool blockedFR = false;
bool blockedRL = false;
bool blockedRR = false;

bool batteryLow = true;
const float batteryThreshold = 7.2f;

const string heading = "--|--N--|--|--E--|--|--S--|--|--W--|--|--N--|--";


// Handler for POSIX signals
void signalHandler(int signo)
{
  if (signo == SIGINT){
    running = false;
  }
}

// Heading display on LCD
void displayHeading(Jhd1313m1 *lcd, Hmc5883l *compass){

    // You can set your declination in radians for more accurate readings
    //compass->set_declination(0.2749);

    while(running){
        compass->update();

        // Round the heading returned by compass and turn it into an index
        // from 0 to 36 for the heading string
        int hdg = int(compass->heading() + 0.5f);
        int hdg_index = hdg / 10;

        // Only write if we get a valid index, otherwise compass is not connected or
        // some error occurred
        if(hdg_index >= 0 && hdg_index <= 36)
        {
            // Write the heading on the first line, lcd->clear() is not needed since
            // we write the entire line
            crit.lock();
            lcd->setCursor(0, 0);
            if (lcd->write("HDG: " + heading.substr(hdg_index, 11)) != UPM_SUCCESS)
                cerr << "MRAA cannot display heading!" << endl;
            crit.unlock();
        }

        // Update readings and display every 250 ms
        usleep(250000);
    }
}

// Battery level display on LCD
void displayBattery(Jhd1313m1 *lcd, GroveVDiv *divider){

    // Variable used for flashing LCD red when battery is low
    uint8_t red = 0x3F;

    while (running) {
        // Read 50 samples each with 2 ms in between (100 ms total)
        int avgValue = divider->value(50);
        // Convert the value to voltage at 3x gain and 5V reference
        float voltage = divider->computedValue(3, avgValue);
        string displayStr = to_string(voltage);
        displayStr = displayStr.substr(0, 4);

        // Write the battery voltage on the second line of the display
        crit.lock();
        lcd->setCursor(1, 0);
        if (lcd->write("Batt: " + displayStr + " V    ") != UPM_SUCCESS)
            cerr << "MRAA cannot display battery voltage!" << endl;
        crit.unlock();

        // Battery low, flash LCD and refresh more often
        if(voltage < batteryThreshold)
        {
            batteryLow = true;
            lcd->setColor(red, 0x00, 0x00);
            // Toggle red bits
            red = ~red;
            sleep(2);
        }
        else{
            // Battery was considered low but new reading is above threshold
            // Thus return to normal operation mode and make the LCD green
            if(batteryLow){
                lcd->setColor(0x00, 0xCF, 0x00);
            }
            batteryLow = false;
            // Refresh every 5 seconds
            sleep(5);
        }
    }
}

// IR sensors thread
void distanceIR(GroveMD *motors, RFR359F *fl, RFR359F *fr, RFR359F *rl, RFR359F *rr){
    while(running){
        // Set the corresponding sensor variable when an object is detected
        if(fl->objectDetected()){
            blockedFL = true;
        }
        else{
            blockedFL = false;
        }
        if(fr->objectDetected()){
            blockedFR = true;
        }
        else{
            blockedFR = false;
        }
        if(rl->objectDetected()){
            blockedRL = true;
        }
        else{
            blockedRL = false;
        }
        if(rr->objectDetected()){
            blockedRR = true;
        }
        else{
            blockedRR = false;
        }
        // Stop the motors if any sensor was triggered
        if(blockedFL || blockedFR || blockedRL || blockedRR){
            motors->setMotorSpeeds(0, 0);
        }
        // Refresh every 10 ms
        usleep(10000);
    }
}

// check if running as root
void checkRoot(void)
{
    int euid = geteuid();
    if (euid) {
        cerr << "This project uses Mraa I/O operations, but you're not running as 'root'.\n"
                "The IO operations below might fail.\n"
                "See the project's Readme for more info.\n\n";
    }
    return;
}


int main(int argc, char **argv)
{

  // check if running as root
  checkRoot();

  // Register signal handler
  signal(SIGINT, signalHandler);

  // Instantiate an I2C Grove Motor Driver on default I2C bus
  // This will also set the I2C bus to 100 kHz for compatibility
  // Default address of 0x0f is used (all 4 switches in up position)
  GroveMD *motors = new GroveMD(GROVEMD_I2C_BUS, GROVEMD_DEFAULT_I2C_ADDR);
  if(motors == NULL){
      cerr << "Failed to initialize the motor driver." << endl;
      exit(EXIT_FAILURE);
  }

  // Initialize the Grove RGB Backlit LCD using implicit
  // 0x62 for RGB_ADDRESS and 0x3E for LCD_ADDRESS
  Jhd1313m1 *lcd = new upm::Jhd1313m1(0);
  if(lcd == NULL){
      cerr << "Failed to initialize the LCD." << endl;
      exit(EXIT_FAILURE);
  }

  // Instantiate the HMC5883 compass on default I2C bus
  // The compass requires an I2C level translator to 3.3V if used
  Hmc5883l *compass = new Hmc5883l(0);
  if(compass == NULL){
      cerr << "Failed to initialize the HMC5883 compass." << endl;
      exit(EXIT_FAILURE);
  }

  // Instantiate the Grove Voltage Divider on pin A0
  GroveVDiv *divider = new GroveVDiv(0);
  if(divider == NULL){
      cerr << "Failed to initialize the voltage divider." << endl;
      exit(EXIT_FAILURE);
  }

  // Instantiate 4 Grove IR Distance Interrupters on pins D2, D4, D6, D8
  RFR359F *frontLeftIR = new RFR359F(2);
  RFR359F *frontRightIR = new RFR359F(4);
  RFR359F *rearLeftIR = new RFR359F(6);
  RFR359F *rearRightIR = new RFR359F(8);

  if(frontLeftIR == NULL || frontRightIR == NULL || rearLeftIR == NULL || rearRightIR == NULL){
      cerr << "Failed to initialize one of the IR sensors." << endl;
      exit(EXIT_FAILURE);
  }

  // Start independent threads for the different components
  // If you're not using a component comment out the corresponding thread call
  thread comp (displayHeading, lcd, compass);
  thread batt (displayBattery, lcd, divider);
  thread collision (distanceIR, motors, frontLeftIR, frontRightIR, rearLeftIR, rearRightIR);

  // Main event and control loop
  // Commands are given through stdin as tuples of the form <CMD_NAME SPEED>
  // Except for the <stop> command and are case sensitive
  while(running){

      string command;
      int speed;

      // Wait command from stdin
      cin.clear();
      cin >> command;
      if(command == "stop" || command.empty()){
          // Stop rover
          motors->setMotorSpeeds(0, 0);
          cout << "Rover stopping!" << endl;
          continue;
      }
      // Read speed
      cin >> speed;
      // Check against non-numbers entered in string
      if(cin.fail()){
          cerr << "Error: Bad input! Please check your command and try again." << endl;
          continue;
      }
      // Check for proper range
      if(speed < 0 || speed > 255){
          cerr << "Error: Speed needs to be between 0 to 255." << endl;
          continue;
      }
      // Direction is set based on command/keyword
      if(command == "fwd" && (!blockedFL || !blockedFR)){
          motors->setMotorDirections(GroveMD::DIR_CCW, GroveMD::DIR_CW);
          motors->setMotorSpeeds(speed, speed);
          cout << "Rover going forward at speed " << speed << endl;
      }
      else if(command == "left"  && (!blockedFL || !blockedRL)){
          motors->setMotorDirections(GroveMD::DIR_CCW, GroveMD::DIR_CCW);
          motors->setMotorSpeeds(speed, speed);
          cout << "Rover turning left at speed " << speed << endl;
      }
      else if(command == "right" && (!blockedFR || !blockedRR)){
          motors->setMotorDirections(GroveMD::DIR_CW, GroveMD::DIR_CW);
          motors->setMotorSpeeds(speed, speed);
          cout << "Rover turning right at speed " << speed << endl;
      }
      else if(command == "rev"  && (!blockedRL || !blockedRR)){
          motors->setMotorDirections(GroveMD::DIR_CW, GroveMD::DIR_CCW);
          motors->setMotorSpeeds(speed, speed);
          cout << "Rover in reverse at speed " << speed << endl;
      }
      else{
          motors->setMotorSpeeds(0, 0);
          cout << "Command not supported or direction blocked!" << endl;
      }
  }

  // Wait for threads to join back
  // If you're not using a component comment out the corresponding join call
  comp.join();
  batt.join();
  collision.join();

  cout << "Exiting..." << endl;

  // Clean up and exit
  delete lcd;
  delete compass;
  delete divider;
  delete motors;
  return 0;
}
