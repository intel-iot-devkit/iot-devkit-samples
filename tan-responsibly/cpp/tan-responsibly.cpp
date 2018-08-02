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
#include <jhd1313m1.hpp>
#include <guvas12d.hpp>
#include <buzzer.hpp>
#include <buzzer_tones.h>
#include <string>

using namespace mraa;
using namespace std;

/**
 * This example helps you to avoid sunburn.
 */

/*
 * Some colors for LCD background
 */
#define LIME_GREEN 50,205,50
#define YELLOW     255,255,0
#define ORANGE     255,165,0
#define RED        255,0,0
#define VIOLET     238,130,238
/*
 * From http://www.seeedstudio.com/wiki/Grove_-_UV_Sensor
 * The output and the UV index is linear:
 * illumination intensity = 307 * Vsig
 * where:
 * Vsig is the value of voltage measured from the SIG pin of the Grove interface, unit V.
 * illumination intensity unit: mW/m2 for the combination strength of UV light with wavelength range: 240nm~370nm
 */
#define GUVA_RAW_TO_INTENSITY_COEFF 307

#define PLATFORM_VOLTAGE            5   // V
#define UV_SAMPLE_PER_QUERY         100
#define UV_INTENSITY_TO_INDEX_COEFF 200
#define UV_INDEX_THRESHOLD          8
#define TEMPERATURE_THRESHOLD       30

// Define the following if using a Grove Pi Shield for UP2 board
#define USING_GROVE_PI_SHIELD
/*
 * Check dangerous conditions and kindly remind the user about risk of sunburn.
 * Use LCD and buzzer to notify warning conditions.
 */
void check_warning_conditions(upm::GUVAS12D *UV_sensor,
    upm::GroveTemp* temperature_sensor, upm::Buzzer* buzzer,
    upm::Jhd1313m1 *lcd) {

  float raw_uvIntensity = 0;
  int temperature = 0;
  std::stringstream row_1, row_2; //LCD rows

  // Update UV intensity and temperature values
  raw_uvIntensity = UV_sensor->value(PLATFORM_VOLTAGE, UV_SAMPLE_PER_QUERY);
  temperature = temperature_sensor->value();

  // Calculate real UV intensity and estimate intensity index
  // See https://en.wikipedia.org/wiki/Ultraviolet_index
  float uvIntensity = raw_uvIntensity * GUVA_RAW_TO_INTENSITY_COEFF;
  int uvIndex = uvIntensity / UV_INTENSITY_TO_INDEX_COEFF;

  // Display UV intensity and temperature values on LCD
  row_1 << "Temp: " << temperature << "    ";
  lcd->setCursor(0, 0);
  if (lcd->write(row_1.str()) != UPM_SUCCESS)
      cerr << "MRAA cannot write temperature!" << endl;

  // Remind possible risk (time to sunburn)
  lcd->setCursor(1, 0);

  if (uvIndex <= 4) {
    row_2 << "UV: " << uvIntensity << "(" << uvIndex << ")    ";
  } else if (uvIndex > 4 && uvIndex <= 7) {
    row_2 << "Sunburn in 30 m";
  } else if (uvIndex > 7 && uvIndex <= 9) {
    row_2 << "Sunburn in 20 m";
  } else if (uvIndex > 9) {
    row_2 << "Sunburn in 10 m";
  }

  lcd->write(row_2.str());

  // Color the display according UV index safety

  if (uvIndex <= 2) {
    lcd->setColor(LIME_GREEN);
  } else if (uvIndex > 2 && uvIndex <= 5) {
    lcd->setColor(YELLOW);
  } else if (uvIndex > 5 && uvIndex <= 7) {
    lcd->setColor(ORANGE);
  } else if (uvIndex > 7 && uvIndex <= 10) {
    lcd->setColor(RED);
  } else if (uvIndex >= 10) {
    lcd->setColor(VIOLET);
  }

  if (uvIndex >= UV_INDEX_THRESHOLD || temperature > TEMPERATURE_THRESHOLD) {
    buzzer->playSound(BUZZER_DO, 1000000);
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

int initPlatform(int& aPinIn1, int& aPinIn2, int& dPinOut, int& i2cPort)
{
	// check which board we are running on
	Platform platform = getPlatformType();
	switch (platform) {
	case INTEL_UP2:
		i2cPort = 0; // I2C
#ifdef USING_GROVE_PI_SHIELD //Offset port number by 512
		aPinIn1 = 1 + 512; // A1
		aPinIn2 = 2 + 512; // A2
		dPinOut = 3 + 512; // D3
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

int main() {
  // check if running as root
  checkRoot();

  int aPinIn1, aPinIn2, dPinOut, i2cPort;
  if (initPlatform(aPinIn1, aPinIn2, dPinOut, i2cPort) == -1)
      cerr << "Not using Grove provide your pinout" << endl;
  
#ifdef USING_GROVE_PI_SHIELD
  addSubplatform(GROVEPI, "0");
#endif

  // UV sensor connected
  upm::GUVAS12D *UV_sensor = new upm::GUVAS12D(aPinIn1);

  // Temperature sensor
  upm::GroveTemp* temp_sensor = new upm::GroveTemp(aPinIn2);

  // Buzzer connected
  upm::Buzzer* buzzer = new upm::Buzzer(dPinOut);

  // LCD connected
  upm::Jhd1313m1* lcd = new upm::Jhd1313m1(i2cPort);

  // Simple error checking
  if ((UV_sensor == NULL) || (temp_sensor == NULL) || (buzzer == NULL)
      || (lcd == NULL)) {
    std::cerr << "Can't create all objects, exiting" << std::endl;
    return mraa::ERROR_UNSPECIFIED;
  }

  buzzer->stopSound();

  // Loop forever checking warning conditions
  for (;;) {
    check_warning_conditions(UV_sensor, temp_sensor, buzzer, lcd);
    sleep(1);
  }

  return mraa::SUCCESS;
}
