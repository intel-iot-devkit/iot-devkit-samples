    /*
 * Authors:
 * - Bufalino Andrea <andrea.bufalino@outlook.com>
 * - Carocci Eugenio <carocci.eugenio@gmail.com>
 * - Ranalli Alessandro <ranalli.alessandro1@gmail.com>
 *
 * Copyright (c) 2015 Bufalino Andrea, Carocci Eugenio, Ranalli Alessandro.
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
#include <string>
#include <sstream>

#include <mraa.hpp>
#include <grove.hpp>
#include <jhd1313m1.hpp>
#include <uln200xa.hpp>
#include <ttp223.hpp>


using namespace std;
using namespace mraa;

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

int initPlatform(int& aPinRotary, int& aPinLight, int& dPinButton, int& i2cPort)
{
    //Update these values as per the hardware board being used.
    aPinRotary = 1,
    aPinLight = 2,
    dPinButton = 4,
    i2cPort = 0;
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_UP2:
#ifdef USING_GROVE_PI_SHIELD //512 offset needed for the shield
        aPinRotary += 512;     // A1 Connector
        aPinLight += 512;      // A2 Connector
        dPinButton += 512;     // D4 Connector
        break;
#else
        return -1;
#endif
    default:
        string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n\n";
        cerr << unknownPlatformMessage;
    }
    return 0;
}


// define the following if not using a board with all the required hardware
//#define SIMULATE_DEVICES

// Speed of the stepper motor in revolutions per minute (RPM).
const int STEPPER_MOTOR_SPEED = 5;
// Number of steps per full revolution.
const int STEPPER_MOTOR_STEPS_FULL_REVOLUTION = 4096;
// Steps required to draw the curtain completely.
const int STEPPER_MOTOR_MAX_STEPS = 2 * STEPPER_MOTOR_STEPS_FULL_REVOLUTION;
/*
 * Number of steps made by the stepper motor for each activation.
 * To complete a whole revolution 4096 steps are required, so using 1024
 * steps, for each activation 1/4 revolution is made.
 */
const int STEPPER_MOTOR_ACTIVATION_STEPS = 1024;

/* Lux threshold. Small light fluctuations can cause continuous activity
 * of the stepper motor.
 * To prevent this behavior a small hysteresis of few lux units is
 * integrated.
 */
const int THRESHOLD = 5;

// Position of stepper motor.
static int stepper_motor_current_step = 0;

/**
 * @brief Get the desired lux target value from the user.
 * @param rotary_sensor Rotary angle sensor.
 * @param light_sensor  Light sensor.
 * @param button    Button.
 * @param lcd       LCD.
 * @return Lux value setted by the user through rotary angle sensor.
 */
int setup_lux_target(upm::GroveRotary *rotary_sensor,
    upm::GroveLight *light_sensor, upm::GroveButton *button,
    upm::Jhd1313m1 *lcd) {
  int lux_target = 0;
  int button_value = 0;
  bool confirmed = false;

  while (!confirmed) {
    /*
     * Rotary_sensor->abs_deg() in within the interval [0-300].
     * Dividing Rotary_sensor->abs_deg() by 6 interval [0-60] is
     * obtained.
     */
#ifndef SIMULATE_DEVICES
    lux_target = rotary_sensor->abs_deg() / 5;
#else
    lux_target = 300/5;
#endif
    lux_target = rotary_sensor->abs_deg() / 5;
    string lux_target_str = static_cast<ostringstream *>(&(ostringstream()
        << lux_target))->str();
    lcd->clear();
    lcd->setCursor(0, 0);
    if (lcd->write("Btn to confirm") != UPM_SUCCESS) {
        cerr << "MRAA cannot write the first string!" << endl;
        return MRAA_ERROR_UNSPECIFIED;
    }
    lcd->setCursor(1, 0);
    lcd->cursorBlinkOn();
    if (lcd->write("Lux Target: " + lux_target_str) != UPM_SUCCESS) {
        cerr << "MRAA cannot write the second string!" << endl;
        return MRAA_ERROR_UNSPECIFIED;
    }

#ifdef SIMULATE_DEVICES
    button_value = 1;
#else
    button_value = button->value();
#endif
    // The button has been pressed
    if (button_value == 1) {
      confirmed = true;
      lcd->cursorBlinkOff();
    }
    usleep(250000);
  }

  return lux_target;
}

/**
 * @brief Show on the LCD the current lux value sensed and the lux target value.
 * @param lcd LCD.
 * @param lux_target Lux target value.
 * @param lux_current Lux current value.
 */
void show_on_lcd(upm::Jhd1313m1 *lcd, int lux_target, int lux_current) {
  stringstream row_1, row_2; //LCD rows
  row_1 << "Lux Current: " << lux_current;
  row_2 << "Lux Target:  " << lux_target;
  lcd->clear();
  lcd->setCursor(0, 0);
  if (lcd->write(row_1.str()) != UPM_SUCCESS) {
      cerr << "MRAA cannot write current lux value!" << endl;
  }
  lcd->setCursor(1, 0);
  if (lcd->write(row_2.str()) != UPM_SUCCESS) {
      cerr << "MRAA cannot write target lux value!" << endl;
  }
}

/**
 * @brief Draw (partially) the curtain, if it is not already completely closed.
 * @param uln200xa Stepper motor driver.
 * @return True if the curtain has been partially closed. False if the curtain
 *     is already completely closed.
 */
bool draw_curtain(upm::ULN200XA *uln200xa) {
  //Check if the curtain is already completely closed.
  if (stepper_motor_current_step != STEPPER_MOTOR_MAX_STEPS) {
    uln200xa->setDirection(ULN200XA_DIR_CW);
    uln200xa->stepperSteps(STEPPER_MOTOR_ACTIVATION_STEPS);
    stepper_motor_current_step += STEPPER_MOTOR_ACTIVATION_STEPS;
    uln200xa->release();
    return true;
  } else {
    //Curtain already completely closed
    return false;
  }
}

/**
 * @brief Open (partially) the curtain, if it is not already completely open.
 * @param uln200xa Stepper motor driver.
 * @return  True if the curtain has been partially opened. False if the
 * curtain is already completely open.
 */
bool open_curtain(upm::ULN200XA *uln200xa) {
  if (stepper_motor_current_step != 0) {
    uln200xa->setDirection(ULN200XA_DIR_CCW);
    uln200xa->stepperSteps(STEPPER_MOTOR_ACTIVATION_STEPS);
    stepper_motor_current_step -= STEPPER_MOTOR_ACTIVATION_STEPS;
    uln200xa->release();
    return true;
  } else {
    //Curtain already completely open.
    return false;
  }
}

/**
 * @brief   Check the current lux value sensed by the light sensor, and if
 *          it is needed, open or draw the curtain.
 * @param light_sensor  Light sensor.
 * @param lcd     LCD.
 * @param uln200xa    Stepper motor driver.
 * @param lux_target  Lux target value.
 */
void check_lux(upm::GroveLight *light_sensor, upm::Jhd1313m1 *lcd,
    upm::ULN200XA *uln200xa, int lux_target) {

  int lux_current=0;
  // Get current lux value sensed by the light sensor.
#ifndef SIMULATE_DEVICES
  lux_current = light_sensor->value();
#else
  lux_current++;
#endif


  show_on_lcd(lcd, lux_target, lux_current);

  if (lux_current > lux_target + THRESHOLD) {
    // Too much light, so draw the curtain.
    cout << "Too much light, so draw the curtain." << endl;
    if (!draw_curtain(uln200xa)) {
      cout << "Curtain already completely closed" << endl;
    }
  } else if (lux_current < lux_target - THRESHOLD) {
    //Too few light, so open the curtain.
    cout << "Too few light, so open the curtain" << endl;
    if (!open_curtain(uln200xa)) {
      cout << "Curtain already completely open" << endl;
    }
  }
}

int main() {

  // check if running as root
  checkRoot();

  /*
   * System can be in two states:
   * - CONFIG: A lux target can be specified from the user.
   * - NORMAL: System try to mantain or reach the lux target value.
   */
  enum State {
    CONFIG, NORMAL
  };

  // Starting state for the system
  State status = CONFIG;

  // Lux target value
  int lux_target;

  //button state value
  int button_value;

#ifndef SIMULATE_DEVICES

  int aPinRotary, aPinLight, dPinButton, i2cPort;
  if (initPlatform(aPinRotary, aPinLight, dPinButton, i2cPort) == -1) {
      cerr << "Not using Grove provide your pinout here" << endl;
  }
#ifdef USING_GROVE_PI_SHIELD
  addSubplatform(GROVEPI, "0");
#endif


  // Instantiate a rotary sensor
  upm::GroveRotary *rotary_sensor = new upm::GroveRotary(aPinRotary);

  // Instantiate a light sensor
  upm::GroveLight *light_sensor = new upm::GroveLight(aPinLight);

  // Instantiate a button
  upm::GroveButton *button = new upm::GroveButton(dPinButton);

  /* Instantiate a stepper motor driver wiring the pins
   * so that I1 is pin D8, I2 is pin D9, I3 is pin D10 and
   * I4 is pin D11
   */
  upm::ULN200XA *uln200xa = new upm::ULN200XA(
      STEPPER_MOTOR_STEPS_FULL_REVOLUTION, 8, 9, 10, 11);

  // LCD connected to the default I2C bus
  upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(i2cPort);

#endif
  // Simple error checking
  if ((rotary_sensor == NULL) || (light_sensor == NULL) || (button == NULL)
      || (uln200xa == NULL) || (lcd == NULL)) {
    cerr << "Can't create all objects, exiting" << endl;
    return mraa::ERROR_UNSPECIFIED;
  }

  uln200xa->setSpeed(STEPPER_MOTOR_SPEED);

  // Loop forever
  for (;;) {

#ifdef SIMULATE_DEVICES
    button_value = 1;
#else
    button_value = button->value();
#endif
    // The button has been pressed
    if (button_value == 1) {
      status = CONFIG;
    }

    if (status == NORMAL) {
      // NORMAL STATE
      check_lux(light_sensor, lcd, uln200xa, lux_target);
    } else {
      // CONFIG STATE
      lux_target = setup_lux_target(rotary_sensor, light_sensor, button, lcd);
      status = NORMAL;
    }
    // Sleep for 500 ms
    usleep(500000);
  }

  return mraa::SUCCESS;
}
