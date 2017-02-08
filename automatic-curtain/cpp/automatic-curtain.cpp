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

/**
 * Open and close automatically a curtain according to the luminosity\n
 * Warning: a 5V power supply is required in order to run this example\n
 * \n
 * First set the wished luminosity with the Grove Rotary Angle Sensor. Then
 * push the Grove Button and the program will try to reach this luminosity by
 * opening/closing the curtain.
 *
 * Hhardware Sensors used:\n
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
 *  Hypothesis:
 * - curtain initially open;
 * - to close completely the courtain 2 stepper motor's full revolutions are
 *   needed;
 * - lux range in [0-60].
 */

using namespace std;

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
  bool confirmed = false;

  while (!confirmed) {
    /*
     * Rotary_sensor->abs_deg() in within the interval [0-300].
     * Dividing Rotary_sensor->abs_deg() by 6 interval [0-60] is
     * obtained.
     */
    lux_target = rotary_sensor->abs_deg() / 5;
    string lux_target_str = static_cast<ostringstream *>(&(ostringstream()
        << lux_target))->str();
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("Btn to confirm");
    lcd->setCursor(1, 0);
    lcd->cursorBlinkOn();
    lcd->write("Lux Target: " + lux_target_str);
    // The button has been pressed
    if (button->value() == 1) {
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
  lcd->write(row_1.str());
  lcd->setCursor(1, 0);
  lcd->write(row_2.str());
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

  int lux_current;
  // Get current lux value sensed by the light sensor.
  lux_current = light_sensor->value();

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
  // Check that we are running on Galileo or Edison
  mraa::Platform platform = mraa::getPlatformType();
  if ((platform != mraa::INTEL_GALILEO_GEN1)
      && (platform != mraa::INTEL_GALILEO_GEN2)
      && (platform != mraa::INTEL_EDISON_FAB_C)) {
    std::cerr << "Unsupported platform, exiting" << std::endl;
    return mraa::ERROR_INVALID_PLATFORM;
  }

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

  // Instantiate a rotary sensor on analog pin A0
  upm::GroveRotary *rotary_sensor = new upm::GroveRotary(0);

  // Instantiate a light sensor on analog pin A3
  upm::GroveLight *light_sensor = new upm::GroveLight(3);

  // Instantiate a button on digital pin D4
  upm::GroveButton *button = new upm::GroveButton(4);

  /* Instantiate a stepper motor driver wiring the pins
   * so that I1 is pin D8, I2 is pin D9, I3 is pin D10 and
   * I4 is pin D11
   */
  upm::ULN200XA *uln200xa = new upm::ULN200XA(
      STEPPER_MOTOR_STEPS_FULL_REVOLUTION, 8, 9, 10, 11);

  // LCD connected to the default I2C bus
  upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0);

  // Simple error checking
  if ((rotary_sensor == NULL) || (light_sensor == NULL) || (button == NULL)
      || (uln200xa == NULL) || (lcd == NULL)) {
    cerr << "Can't create all objects, exiting" << endl;
    return mraa::ERROR_UNSPECIFIED;
  }

  uln200xa->setSpeed(STEPPER_MOTOR_SPEED);

  // Loop forever
  for (;;) {

    // The button has been pressed
    if (button->value() == 1) {
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
