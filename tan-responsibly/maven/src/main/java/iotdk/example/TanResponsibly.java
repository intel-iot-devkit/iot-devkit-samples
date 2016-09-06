/*
 * Author:
 * - Fabio Federici <fabio.federici@univaq.it>
 * - Christian Roschat <christian.roschat@intel.com>
 *
 * Copyright (c) 2016 Fabio Federici.
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
 * @brief Tan responsibly
 *
 * This example helps you to avoid sunburn.
 *
 * @hardware Sensors used:\n
 * Grove UV Sensor connected to the Grove Base Shield Port A0\n
 * Grove Temperature Sensor connected to the Grove Base Shield Port A1\n
 * Grove Buzzer connected to the Grove Base Shield Port D2\n
 * Jhd1313m1 LCD connected to any I2C on the Grove Base Shield
 *
 * @req upm_i2clcd.jar
 * @req upm_grove.jar
 * @req upm_buzzer.jar
 * @req upm_guvas12d.jar
 *
 * @date 21/06/2016
 */
package iotdk.example;

import mraa.Platform;
import mraa.mraa;
import upm_buzzer.Buzzer;
import upm_grove.GroveTemp;
import upm_guvas12d.GUVAS12D;
import upm_i2clcd.Jhd1313m1;

public class TanResponsibly {

  /*
   * Some colors for LCD background
   */
  static final short LIME_GREEN[] = { 50, 205,  50};
  static final short YELLOW[]     = {255, 255,   0};
  static final short ORANGE[]     = {255, 165,   0};
  static final short RED[]        = {255,   0,   0};
  static final short VIOLET[]     = {238, 130, 238};
  /*
   * From http://www.seeedstudio.com/wiki/Grove_-_UV_Sensor
   * The output and the UV index is linear:
   * illumination intensity = 307 * Vsig
   * where:
   * Vsig is the value of voltage measured from the SIG pin of the Grove interface, unit V.
   * illumination intensity unit: mW/m2 for the combination strength of UV light with wavelength range: 240nm~370nm
   */
  static final int GUVA_RAW_TO_INTENSITY_COEFF = 307;

  static final int DO                          = 3800;
  static final int PLATFORM_VOLTAGE            = 5; // in V
  static final int UV_SAMPLE_PER_QUERY         = 100;
  static final int UV_INTENSITY_TO_INDEX_COEFF = 200;
  static final int UV_INDEX_THRESHOLD          = 8;
  static final int TEMPERATURE_THRESHOLD       = 30;

  /*
   * Check dangerous conditions and kindly remind the user about risk of sunburn.
   * Use LCD and buzzer to notify warning conditions.
   */
  static void checkWarningConditions(GUVAS12D UvSensor,
      GroveTemp temperatureSensor, Buzzer buzzer,
      Jhd1313m1 lcd) {

    float rawUVIntensity = 0;
    int temperature = 0;
    String row1, row2 = ""; //LCD rows

    // Update UV intensity and temperature values
    rawUVIntensity = UvSensor.value(PLATFORM_VOLTAGE, UV_SAMPLE_PER_QUERY);
    temperature = temperatureSensor.value();

    // Calculate real UV intensity and estimate intensity index
    // See https://en.wikipedia.org/wiki/Ultraviolet_index
    float uvIntensity = rawUVIntensity * GUVA_RAW_TO_INTENSITY_COEFF;
    int uvIndex = (int) (uvIntensity / UV_INTENSITY_TO_INDEX_COEFF);

    // Display UV intensity and temperature values on LCD
    row1 = "Temp: " + temperature + "    ";
    lcd.setCursor(0, 0);
    lcd.write(row1);

    // Remind possible risk (time to sunburn)
    lcd.setCursor(1, 0);

    if (uvIndex <= 4) {
      row2 = "UV: " + uvIntensity + "(" + uvIndex + ")    ";
    } else if (uvIndex > 4 && uvIndex <= 7) {
      row2 = "Sunburn in 30 m";
    } else if (uvIndex > 7 && uvIndex <= 9) {
      row2 = "Sunburn in 20 m";
    } else if (uvIndex > 9) {
      row2 = "Sunburn in 10 m";
    }

    lcd.write(row2);

    // Color the display according UV index safety
    if (uvIndex <= 2) {
      lcd.setColor(LIME_GREEN[0], LIME_GREEN[1], LIME_GREEN[2]);
    } else if (uvIndex > 2 && uvIndex <= 5) {
      lcd.setColor(YELLOW[0], YELLOW[1], YELLOW[2]);
    } else if (uvIndex > 5 && uvIndex <= 7) {
      lcd.setColor(ORANGE[0], ORANGE[1], ORANGE[2]);
    } else if (uvIndex > 7 && uvIndex <= 10) {
      lcd.setColor(RED[0], RED[1], RED[2]);
    } else if (uvIndex >= 10) {
      lcd.setColor(VIOLET[0], VIOLET[1], VIOLET[2]);
    }

    if (uvIndex >= UV_INDEX_THRESHOLD || temperature > TEMPERATURE_THRESHOLD) {
      buzzer.playSound(DO, 1000000);
    }
  }

  public static void main(String[] args) {
    // check that we are running on Galileo or Edison
    Platform platform = mraa.getPlatformType();
    if (platform != Platform.INTEL_GALILEO_GEN1 &&
        platform != Platform.INTEL_GALILEO_GEN2 &&
        platform != Platform.INTEL_EDISON_FAB_C) {
      System.err.println("Unsupported platform, exiting");
      return;
    }

    // UV sensor connected to A0 (analog in)
    GUVAS12D UvSensor = new GUVAS12D(0);

    // Temperature sensor connected to A1 (analog in)
    GroveTemp temp_sensor = new GroveTemp(1);

    // Buzzer connected to D2 (digital out)
    Buzzer buzzer = new Buzzer(5);

    // LCD connected to the default I2C bus
    Jhd1313m1 lcd = new Jhd1313m1(0);

    // Simple error checking
    if ((UvSensor == null) || (temp_sensor == null) || (buzzer == null)
        || (lcd == null)) {
      System.err.println("Could not create all objects, exiting");
      return;
    }

    buzzer.stopSound();

    // Loop forever checking warning conditions
    for (;;) {
      checkWarningConditions(UvSensor, temp_sensor, buzzer, lcd);
      try {
        Thread.sleep(1000);
      } catch (InterruptedException e) {
        System.err.println("Sleep interrupted: " + e.toString());
      }
    }
  }
}
