/*
 * Author:
 * - Vittoriano Muttillo <vittoriano.muttillo@gmail.com>
 * - Giacomo Valente <giakomo.87v@gmail.com>
 *
 * Copyright (c) 2015 Vittoriano Muttillo, Giacomo Valente.
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

#include <iostream>
#include <unistd.h>

#include <mraa.hpp>
#include <grove.hpp>
#include <jhd1313m1.hpp>
#include <uln200xa.hpp>

/**
 * Move photovoltaic panel following the maximum brightness of the sun.\n\n
 * The idea is to move photovoltaic panel (represented by a motor) according to
 * the movement of the sun, directing it towards the maximum light radiated
 * during the day using the maximum light intensity detected by two light
 * sensor placed parallel to each other.\n
 * The idea is to compute the sensors average light in a calibration step and 
 * use this number to move motor according to the movement of light.
 *
 * Hhardware Sensors used:\n
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
 */

using namespace std;

// Threshold of light
#define THRESHOLD 2

// Steps per revolution
#define STEPS_PER_REV 4096

// Left and right light average static variables
static int lightLAVG, lightRAVG;

void solarTracker(upm::Jhd1313m1* lcd, upm::GroveLight* lightL,
    upm::GroveLight* lightR, upm::ULN200XA* uln200xa) {

  int lightLST, lightRST;

  // Since LCD displays data in string format, we need to convert (light value)
  // from integer to string
  lightLST = lightL->value();
  char tdataL[10];
  lightRST = lightR->value();
  char tdataR[10];
  sprintf(tdataL, "%d", lightLST);
  sprintf(tdataR, "%d", lightRST);

  // Writing light info on LCD
  lcd->setCursor(0, 0);
  lcd->write("Left:          ");
  lcd->setCursor(1, 0);
  lcd->write("Right:          ");
  lcd->setCursor(0, 6);
  lcd->write(tdataL);
  lcd->setCursor(1, 7);
  lcd->write(tdataR);

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
  if ((lightLST < THRESHOLD) && (lightRST) < THRESHOLD) {

    // Lightless state
    lcd->setCursor(0, 0);
    lcd->write("No sun");
    lcd->clear();

    // 2)
  } else if (lightLST < lightLAVG) {

    // a)
    if (lightLST < lightRST) {
      // Rotating 1/32 revolution clockwise
      uln200xa->setDirection(ULN200XA_DIR_CW);
      // b)
    } else if (lightLST > lightRST) {
      // Rotating 1/32 revolution clockwise
      uln200xa->setDirection(ULN200XA_DIR_CCW);
    }

    uln200xa->stepperSteps(128);

    // 3)
  } else if (lightRST < lightRAVG) {

    // a)
    if (lightRST < lightLST) {
      // Rotating 1/32 revolution counter clockwise
      uln200xa->setDirection(ULN200XA_DIR_CCW);
      // b)
    } else if (lightRST > lightLST) {
      // Rotating 1/32 revolution counter clockwise
      uln200xa->setDirection(ULN200XA_DIR_CW);
    }
    uln200xa->stepperSteps(128);

  }
  sleep(1);
}

int main() {
  // Check that we are running on Galileo or Edison
  mraa::Platform platform = mraa::getPlatformType();
  if ((platform != mraa::INTEL_GALILEO_GEN1)
      && (platform != mraa::INTEL_GALILEO_GEN2)
      && (platform != mraa::INTEL_EDISON_FAB_C)) {
    std::cerr << "Unsupported platform, exiting" << std::endl;
    return mraa::ERROR_INVALID_PLATFORM;
  }

  // LCD screen object (the lcd is connected to I2C port, bus 0)
  upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0);

  //Left light sensor object (Analog pin 0)
  upm::GroveLight* lightL = new upm::GroveLight(0);

  //Right light sensor object (Analog pin 1)
  upm::GroveLight* lightR = new upm::GroveLight(1);

  /* Instantiate a Stepper motor on a ULN200XA Dual H-Bridge.
   * Wire the pins so that I1 is pin D6, I2 is D7, I3 is D8 and I4 is D9
   */
  upm::ULN200XA* uln200xa = new upm::ULN200XA(STEPS_PER_REV, 6, 7, 8, 9);

  // Simple error checking
  if ((lcd == NULL) || (lightL == NULL) || (lightR == NULL)
      || (uln200xa == NULL)) {
    std::cerr << "Can't create all objects, exiting" << std::endl;
    return mraa::ERROR_UNSPECIFIED;
  }

  // Writing intro text
  lcd->setCursor(0, 0);
  lcd->write("Smart PV");
  lcd->setCursor(1, 0);
  lcd->write("for Edison Board");

  // Set speed of Grove Gear Stepper Motor with Drive
  uln200xa->setSpeed(7);

  // Calibration Step
  int brightL1, brightL2, brightR1, brightR2;

  // Rotating 1/8 revolution clockwise
  uln200xa->setDirection(ULN200XA_DIR_CW);
  uln200xa->stepperSteps(STEPS_PER_REV / 8);
  brightL1 = lightL->value();
  brightR1 = lightR->value();
  sleep(1);

  // Rotating 1/4 revolution counter clockwise
  uln200xa->setDirection(ULN200XA_DIR_CCW);
  uln200xa->stepperSteps(STEPS_PER_REV / 4);
  brightL2 = lightL->value();
  brightR2 = lightR->value();
  sleep(1);

  // Compute average light sensor value
  lightLAVG = (brightL1 + brightL2) / 2;
  lightRAVG = (brightR1 + brightR2) / 2;

  // Init text on display

  // Loop forever updating the lightL and lightR values every second
  while (true) {
    solarTracker(lcd, lightL, lightR, uln200xa);
  }

  return mraa::SUCCESS;
}
