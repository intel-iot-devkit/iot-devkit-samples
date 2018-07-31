/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
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
 * This project template includes the starting code needed
 * to use the various sensors that come with the Grove Starter Kit.
 * Update the temperature values and reflect the changes on the LCD.
 *
 * Hardware:
 * - Grove LED (GroveLed) connected to the Grove Base Shield Port D3
 * - Grove Button (GroveButton) connected to the Grove Base Shield Port D4
 * - Grove Temperature Sensor (GroveTemp) connected to the Grove Base Shield Port A0
 * - Grove LCD (Jhd1313m1) connected to any I2C on the Grove Base Shield
 *
 * Update the temperature values and reflect the changes on the LCD
 * - change LCD backlight color based on the measured temperature,
 *   a cooler color for low temperatures, a warmer one for high temperatures
 * - display current temperature
 * - record and display MIN and MAX temperatures
 * - reset MIN and MAX values if the button is being pushed
 * - blink the led to show the temperature was measured and data updated
 *
 * Use a platform with I2C, Analog and GPIO capabilities
 */

#include "mraa.hpp"
#include "grove.hpp"
#include "jhd1313m1.hpp"

#include <climits>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace std;
using namespace mraa;

// Define the following if using a Grove Pi Shield for UP2 board
#define USING_GROVE_PI_SHIELD

void temperature_update(upm::GroveTemp* temperature_sensor, upm::GroveButton* button,
        upm::GroveLed* led, upm::Jhd1313m1 *lcd)
{
    // minimum and maximum temperatures registered, the initial values will be
    // replaced after the first read
    static int min_temperature = INT_MAX;
    static int max_temperature = INT_MIN;

    // the temperature range in degrees Celsius,
    // adapt to your room temperature for a nicer effect!
    const int TEMPERATURE_RANGE_MIN_VAL = 18;
    const int TEMPERATURE_RANGE_MAX_VAL = 31;

    // other helper variables
    int temperature; // temperature sensor value in degrees Celsius
    float fade; // fade value [0.0 .. 1.0]
    uint8_t r, g, b; // resulting LCD backlight color components [0 .. 255]
    std::stringstream row_1, row_2; // LCD rows

    // update the min and max temperature values, reset them if the button is
    // being pushed
    temperature = temperature_sensor->value();
    if (button->value() == 1) {
        min_temperature = temperature;
        max_temperature = temperature;
    } else {
        if (temperature < min_temperature) {
            min_temperature = temperature;
        }
        if (temperature > max_temperature) {
            max_temperature = temperature;
        }
    }

    // display the temperature values on the LCD
    row_1 << "Temp " << temperature << "    ";
    row_2 << "Min " << min_temperature << " Max " << max_temperature << "    ";
    lcd->setCursor(0, 0);
    if (lcd->write(row_1.str()) != UPM_SUCCESS)
        cerr << "MRAA cannot display min temperature!" << endl;
    lcd->setCursor(1, 0);
    if (lcd->write(row_2.str()) != UPM_SUCCESS)
        cerr << "MRAA cannot display max temperature!" << endl;

    // set the fade value depending on where we are in the temperature range
    if (temperature <= TEMPERATURE_RANGE_MIN_VAL) {
        fade = 0.0;
    } else if (temperature >= TEMPERATURE_RANGE_MAX_VAL) {
        fade = 1.0;
    } else {
        fade = (float)(temperature - TEMPERATURE_RANGE_MIN_VAL) /
                (TEMPERATURE_RANGE_MAX_VAL - TEMPERATURE_RANGE_MIN_VAL);
    }

    // fade the color components separately
    r = (int)(255 * fade);
    g = (int)(64 * fade);
    b = (int)(255 * (1 - fade));

    // blink the led for 50 ms to show the temperature was actually sampled
    led->on();
    usleep(50000);
    led->off();

    // apply the calculated result
    lcd->setColor(r, g, b);
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

int initPlatform(int& dInPin, int& dOutPin, int& aPin, int& i2cPort)
{
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_UP2:
        i2cPort = 0; 	// I2C
#ifdef USING_GROVE_PI_SHIELD
        dInPin = 4 + 512; 	// D4
        dOutPin = 3 + 512; 	// D3
        aPin = 0 + 512; 	// A0
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

int main()
{
    // check if running as root
    checkRoot();
    int dInPin, dOutPin, aPin, i2cPort;
    if (initPlatform(dInPin, dOutPin, aPin, i2cPort) == -1)
        cerr << "Not using Grove, provide your pinout here" << endl;

#ifdef USING_GROVE_PI_SHIELD
    addSubplatform(GROVEPI, "0");
#endif

    // button connected to (digital in)
    upm::GroveButton* button = new upm::GroveButton(dInPin);

    // led connected to (digital out)
    upm::GroveLed* led = new upm::GroveLed(dOutPin);

    // temperature sensor connected to (analog in)
    upm::GroveTemp* temp_sensor = new upm::GroveTemp(aPin);

    // LCD connected to the I2C bus
    upm::Jhd1313m1* lcd = new upm::Jhd1313m1(i2cPort);

    // simple error checking
    if ((button == NULL) || (led == NULL) || (temp_sensor == NULL) || (lcd == NULL)) {
        cerr << "Can't create all objects, exiting" << endl;
        return ERROR_UNSPECIFIED;
    }

    // loop forever updating the temperature values every second
    for (;;) {
        temperature_update(temp_sensor, button, led, lcd);
        sleep(1);
    }

    return SUCCESS;
}
