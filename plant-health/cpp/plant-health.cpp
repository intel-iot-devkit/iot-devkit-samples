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

using namespace std;
using namespace mraa;

/**
 * Example to simulate a Plant Health Monitoring System.
 *
 * Hardware Sensors used:\n
 * Grove Moisture Sensor connected to the Grove Base Shield Port A0\n
 * Grove Temperature Sensor connected to the Grove Base Shield Port A1\n
 * GUVA-S12D UV sensor to the Grove Base Shield port A2\n
 * Grove Dry-Reed Relay connected to the Grove Base Shield Port D2\n
 * Jhd1313m1 LCD connected to any I2C on the Grove Base Shield
 *
 * Use a platform that has I2C, Analog and GPIO capabilities
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
  if (lcd->write(row_1.str()) != UPM_SUCCESS)
      cerr << "MRAA cannot display row 1!" << endl;
  lcd->setCursor(1, 0);
  if (lcd->write(row_2.str()) != UPM_SUCCESS)
      cerr << "MRAA cannot display row 2!" << endl;
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


int main() {

  // check if running as root
  checkRoot();

  int aPin0 = 0,
      aPin1 = 1,
      aPin2 = 2,
      dPin2 = 2,
      i2cPort = 0;
  string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
      "you are running it on an unrecognized platform. "
      "You may need to modify the MRAA/UPM initialization code to "
      "ensure it works properly on your platform.\n\n";
  // check which board we are running on
  Platform platform = getPlatformType();
  switch (platform) {
    case INTEL_UP2:
#ifdef USING_GROVE_PI_SHIELD //Needs offset by 512
      aPin0 += 512;  // A0
      aPin1 += 512;  // A1
      aPin2 += 512;  // A2
      dPin2 += 512;  // D2
      break;
#else
      cerr << "Not using Grove Pi Shield, provide your pinout here" << endl;
      return -1;
#endif
    default:
          cerr << unknownPlatformMessage;
  }
#ifdef USING_GROVE_PI_SHIELD
  addSubplatform(GROVEPI, "0");
#endif

  // Moisture sensor connected to A0 (analog in)
  upm::GroveMoisture* moisture_sensor = new upm::GroveMoisture(aPin0);

  // Temperature sensor connected to A1 (analog in)
  upm::GroveTemp* temp_sensor = new upm::GroveTemp(aPin1);

  // UV sensor connected to A2 (analog in)
  upm::GUVAS12D *UV_sensor = new upm::GUVAS12D(aPin2);

  // Dry Reed relay connected to D2 (digital out)
  upm::GroveRelay* dry_Reed_relay = new upm::GroveRelay(dPin2);

  // LCD connected to the default I2C bus
  upm::Jhd1313m1* lcd = new upm::Jhd1313m1(i2cPort);

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
