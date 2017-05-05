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
 * @file
 * @ingroup grove
 * @brief API to Pulse Width Modulation
 *
 * Demonstrate how to use PWM with an output pin using the MRAA library.
 * If the output is connected to a led, its intensity, as perceived by the human
 * eye, will vary depending on the duty cycle.
 * A suitable part to use this example with in the Grove Starter Kit is the LED.
 *
 * @hardware - digital out: output connected to digital pin 6 (Grove Base Shield Port D6)
 *
 * @req mraa.jar
 *
 * @date 15/06/2016
 */
package iotdk.example;

import mraa.Platform;
import mraa.Pwm;
import mraa.Result;
import mraa.mraa;

public class PWM {

  public static void main(String[] args) {
    // check that we are running on Galileo or Edison
    Platform platform = mraa.getPlatformType();
    if (platform != Platform.INTEL_GALILEO_GEN1 &&
        platform != Platform.INTEL_GALILEO_GEN2 &&
        platform != Platform.INTEL_EDISON_FAB_C &&
        platform != Platform.MRAA_INTEL_JOULE_EXPANSION) {
      System.err.println("Unsupported platform, exiting");
      return;
    }

    // create a PWM object from MRAA using pin 6
    // note that not all digital pins can be used for PWM, the available ones
    // are usually marked with a ~ on the board's silk screen
    Pwm pwm_pin = null;
    pwm_pin = new Pwm(6);
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