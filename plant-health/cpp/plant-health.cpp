/*
 * Author: Fabio Federici <fabio.federici@univaq.it>
 * Copyright (c) 2015 Fabio Federici.
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
#include <sstream>

#include <mraa.hpp>
#include <grove.hpp>
#include <grovemoisture.hpp>
#include <mq5.hpp>
#include <jhd1313m1.hpp>
#include <guvas12d.hpp>

/**
 * Example to simulate a Plant Health Monitoring System.
 *
 * Hardware Sensors used:\n
 * Grove Moisture Sensor connected to the Grove Base Shield Port A0\n
 * Grove Temperature Sensor connected to the Grove Base Shield Port A1\n
 * GUVA-S12D UV sensor to the Grove Base Shield port A2\n
 * Grove Dry-Reed Relay connected to the Grove Base Shield Port D2\n
 * Jhd1313m1 LCD connected to any I2C on the Grove Base Shield
 */

/*
 * Define some color for LCD background
 */
#define LIME_GREEN         50,205,50
#define RED                255,0,0
#define LIGHT_STEEL_BLUE   176,196,222

/*
 * From http://www.seeedstudio.com/wiki/Grove_-_UV_Sensor
 * The output and the UV index is linear:
 * illumination intensity = 307 * Vsig
 * where:
 * Vsig is the value of voltage measured from the SIG pin of the Grove interface, unit V.
 * illumination intensity unit: mW/m2 for the combination strength of UV light with wavelength range: 240nm~370nm
 */
#define GUVA_RAW_TO_INTENSITY_COEFF 307

/*
 * Settings
 */
#define PLATFORM_VOLTAGE    5   // V
#define UV_SAMPLE_PER_QUERY 100
#define MOISTURE_MIN_THRESHOLD  300 // 0-300, sensor in air or dry soil
#define TEMP_MIN_THRESHOLD  18  // Celsius
#define TEMP_MAX_THRESHOLD  30  // Celsius
#define UV_MIN_THRESHOLD    50  // mW/m2
#define PUMP_DURATION       10  // seconds

/*
 * This function checks soil, temperature and light conditions.
 * Measured data are printed to LCD display.
 * In case of dry soil, a dry Reed relay driving a water pump is activated.
 * Warning conditions are signaled by a red LCD background.
 */
void monitor_plant_conditions(upm::GroveMoisture *moisture_sensor,
    upm::GroveTemp *temp_sensor, upm::GUVAS12D *UV_sensor,
    upm::GroveRelay *dry_Reed_relay, upm::Jhd1313m1 *lcd) {
  std::stringstream row_1, row_2; // LCD rows

  // Read moisture and temperature value
  int moisture = moisture_sensor->value();
  int temperature = temp_sensor->value();

  // Get UV intensity
  float raw_uvIntensity = UV_sensor->value(PLATFORM_VOLTAGE,
  UV_SAMPLE_PER_QUERY);
  float intensity = raw_uvIntensity * GUVA_RAW_TO_INTENSITY_COEFF;

  // In case of dry soil, activate the pump for a fixed time

  if (moisture < MOISTURE_MIN_THRESHOLD) {
    lcd->setColor(LIGHT_STEEL_BLUE);

    row_1.str("Dry soil!       ");
    row_2.str("Watering...     ");
    lcd->setCursor(0, 0);
    lcd->write(row_1.str());
    lcd->setCursor(1, 0);
    lcd->write(row_2.str());

    dry_Reed_relay->on();
    sleep(PUMP_DURATION);
    dry_Reed_relay->off();
  }

  // If any of the measured parameters is out of range, display it in red

  if (temperature < TEMP_MIN_THRESHOLD || temperature > TEMP_MAX_THRESHOLD
      || intensity < UV_MIN_THRESHOLD) {
    lcd->setColor(RED);
  } else {
    lcd->setColor(LIME_GREEN);
  }

  // Display the temperature and intensity data
  row_1.str("");
  row_2.str("");
  row_1 << "Temperature: " << temperature << "  ";
  row_2 << "Light: " << intensity << "   ";
  lcd->setCursor(0, 0);
  lcd->write(row_1.str());
  lcd->setCursor(1, 0);
  lcd->write(row_2.str());
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

  // Moisture sensor connected to A0 (analog in)
  upm::GroveMoisture* moisture_sensor = new upm::GroveMoisture(0);

  // Temperature sensor connected to A1 (analog in)
  upm::GroveTemp* temp_sensor = new upm::GroveTemp(1);

  // UV sensor connected to A2 (analog in)
  upm::GUVAS12D *UV_sensor = new upm::GUVAS12D(2);

  // Dry Reed relay connected to D2 (digital out)
  upm::GroveRelay* dry_Reed_relay = new upm::GroveRelay(2);

  // LCD connected to the default I2C bus
  upm::Jhd1313m1* lcd = new upm::Jhd1313m1(0);

  // Simple error checking
  if ((moisture_sensor == NULL) || (temp_sensor == NULL) || (UV_sensor == NULL)
      || (dry_Reed_relay == NULL) || (lcd == NULL)) {
    std::cerr << "Can't create all objects, exiting" << std::endl;
    return mraa::ERROR_UNSPECIFIED;
  }

  for (;;) {
    monitor_plant_conditions(moisture_sensor, temp_sensor, UV_sensor,
        dry_Reed_relay, lcd);
    sleep(15);
  }

  return mraa::SUCCESS;
}
