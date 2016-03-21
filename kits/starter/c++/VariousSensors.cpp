/*
 * Author: Sisinty Sasmita Patra <sisinty.s.patra@intel.com>
 * Copyright (c) 2015-2016 Intel Corporation.
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
 * @brief Various Sensors
 *
 * This template provides a framework and includes sample code to show
 * you how to use various sensors that are available in the starter kit.
 *
 * @cc
 * @cxx
 * @ld -lupm-i2clcd -lupm-grove -lupm-mma7660 -lupm-ttp223
 *
 * @date 07/06/2015
 */

#include "jhd1313m1.h"   // Lcd Display
#include "grove.h"       // Button, Temp, Light, Rotary Sensors
#include "mma7660.h"     // 3-Axis Digital Accelerometer
#include "ttp223.h"      // Touch Sensor

#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

using namespace std;
using namespace upm;

int
main(int argc, char **argv)
{

    // Create the temperature sensor object using AIO pin 1
    upm::GroveTemp* temp = new upm::GroveTemp(1);

    // Create the lcd sensor object using I2C pin
    upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62);

    // Create the button object using GPIO pin 2
    upm::GroveButton* button = new upm::GroveButton(2);

    // Create the rotary Sensor object using AIO pin 0
    upm::GroveRotary* knob = new upm::GroveRotary(0);

    // Create the light sensor object using AIO pin 2
    upm::GroveLight* light = new upm::GroveLight(2);

    // Create the TTP223 touch sensor object using GPIO pin 4
    upm::TTP223* touch = new upm::TTP223(4);

    // Create the MMA7660 accelerometer on I2C bus 0
    upm::MMA7660 *accel = new upm::MMA7660(MMA7660_I2C_BUS,
        MMA7660_DEFAULT_I2C_ADDR);

    // place device in standby mode so we can write registers
    accel->setModeStandby();

    // enable 64 samples per second
    accel->setSampleRate(upm::MMA7660::AUTOSLEEP_64);

    // place device into active mode
    accel->setModeActive();

    int celsius;
    float rotary;
    int lit;
    bool tuch;
    float ax, ay, az;

    // This for loop is required to get the updated value from respective sensors
    for (int i=0; i < 5; i++) {
        celsius = temp->value();
        rotary = knob->abs_deg();
        lit = light->value();
        tuch = touch->isPressed();
        accel->getAcceleration(&ax, &ay, &az);
    }

    lcd->setCursor(0,0);
    lcd->write("welcome to ");
    lcd->setCursor(1,0);
    lcd->write("Starter Kit!");

    sleep(3);
    lcd->clear();

    /* This variable helps to display one particular sensor at any given
     * time, when button value is '1' */
    int x = 0;

    lcd->setCursor(0,0);
    lcd->write("Press Button ");

    /* This while loop continously checks for button value.
     * if button value is '1', the sensors values are displayed depending
     * on x value
     */

    while(1)
    {
        if(button->value() != 0)
        {
            if(x == 0)
            {
                celsius = temp->value();

                // Since LCD displays data in string format,
                // we need to convert (celsius value) from integer to string
                string tdata = static_cast<ostringstream*>(
                    &(ostringstream() << celsius))-> str();
                lcd->clear();
                sleep(1);

                lcd->setCursor(0,0);
                lcd->write("Temperature in ");
                lcd->setCursor(1,2);
                lcd->write("celsius: ");
                lcd->write(tdata);

                sleep(3);
                lcd->clear();
                x += 1;

                lcd->write("Press Button");
            }
            else if(x == 1)
            {
                rotary = knob->abs_deg();
                string rotdata = static_cast<ostringstream*>(
                    &(ostringstream() << rotary))-> str();
                lcd->clear();
                sleep(1);

                lcd->setCursor(0,0);
                lcd->write("Rotatory Angle ");
                lcd->setCursor(1,2);
                lcd->write("in degree: ");
                lcd->write(rotdata);

                sleep(3);
                lcd->clear();
                x += 1;

                lcd->write("Press Button");
            }
            else if(x == 2)
            {
                lit = light->value();
                string litdata = static_cast<ostringstream*>(
                    &(ostringstream() << lit))-> str();
                lcd->clear();
                sleep(1);

                lcd->setCursor(0,0);
                lcd->write("light value ");
                lcd->setCursor(1,2);
                lcd->write("in lux: ");
                lcd->write(litdata);

                sleep(3);
                lcd->clear();
                x += 1;

                lcd->write("Press Button");
            }
            else if(x == 3)
            {
                tuch = touch->isPressed();
                string touchdata = static_cast<ostringstream*>(
                    &(ostringstream() << tuch))-> str();
                lcd->clear();
                sleep(1);

                lcd->setCursor(0,0);
                lcd->write("Touch Sensor ");
                lcd->setCursor(1,2);
                lcd->write("is pressed: ");
                lcd->write(touchdata);

                sleep(3);
                lcd->clear();
                x += 1;

                lcd->write("Press Button");
            }
            else if(x == 4)
            {
                accel->getAcceleration(&ax, &ay, &az);
                string axdata = static_cast<ostringstream*>(
                    &(ostringstream() << ax))-> str();
                string aydata = static_cast<ostringstream*>(
                    &(ostringstream() << ay))-> str();
                string azdata = static_cast<ostringstream*>(
                    &(ostringstream() << az))-> str();

                sleep(1);
                lcd->setCursor(0,0);
                lcd->write("Acceleration x: ");
                lcd->setCursor(1,2);
                lcd->write(axdata);
                sleep(1);

                lcd->clear();

                sleep(1);
                lcd->setCursor(0,0);
                lcd->write("Acceleration y: ");
                lcd->setCursor(1,2);
                lcd->write(aydata);
                sleep(1);

                lcd->clear();

                sleep(1);
                lcd->setCursor(0,0);
                lcd->write("Acceleration z: ");
                lcd->setCursor(1,2);
                lcd->write(azdata);

                sleep(3);
                lcd->clear();
                x = 0;

                lcd->write("Press Button");
            }
        }
    }

    // Delete button sensor object
    delete button;
    // Delete temperature sensor object
    delete temp;
    // Delete rotary angle sensor object
    delete knob;
    // Delete light sensor object
    delete light;
    // Delete touch sensor object
    delete touch;
    // Delete accelerometer object
    delete accel;

    return 0;
}
