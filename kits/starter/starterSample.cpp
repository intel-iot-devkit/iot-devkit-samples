/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Contributions: Sarah Knepper <sarah.knepper@intel.com>
 * Copyright (c) 2014 Intel Corporation.
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

#include "jhd1313m1.h"   //lcd display
#include "grove.h"       //button, temp, light, rotary Sensor
#include "adxl345.h"     //3-Axis digital accelerometer
#include "buzzer.h"      //buzzer
#include "mic.h"         //sound sensor
#include "at42qt1070.h"  //Touch sensor
#include "ldt0028.h"     //Piezo vibration sensor

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

    int celsius;
    float rotary;
    int lit;

    // This for loop is required to get the updated value from respective sensors
    for (int i=0; i < 5; i++) {
    	celsius = temp->value();
    	rotary = knob->abs_deg();
    	lit = light->value();
    }

    lcd->setCursor(0,0);
    lcd->write("welcome to ");
    lcd->setCursor(1,0);
    lcd->write("Starter Kit!");

    sleep(3);
    lcd->clear();

    int x = 0;
    while(1)
    {
    	if(button->value() == 0)
    	{
    	    lcd->setCursor(0,0);
    	    lcd->write("Press Button ");
    	}
    	else
    	{
    		if(x == 0)
    		{
    			celsius = temp->value();
    			string tdata = static_cast<ostringstream*>( &(ostringstream() << celsius))-> str();
    			lcd->clear();
    			lcd->setCursor(0,0);
    			lcd->write("Temperature in ");
    			lcd->setCursor(1,2);
    			lcd->write("celsius: ");
    			lcd->write(tdata);

    			sleep(3);
    			lcd->clear();
    			x += 1;
    		}

    		else if(x == 1)
    		{
    			rotary = knob->abs_deg();
    			string rotdata = static_cast<ostringstream*>( &(ostringstream() << rotary))-> str();
    			lcd->clear();
    			lcd->setCursor(0,0);
    			lcd->write("Rotatory Angle ");
    			lcd->setCursor(1,2);
    			lcd->write("degree: ");
    			lcd->write(rotdata);

    			sleep(3);
    			lcd->clear();
    			x += 1;
    		}

    		else if(x == 2)
    		{
    			lit = light->value();
    			string litdata = static_cast<ostringstream*>( &(ostringstream() << lit))-> str();
    			lcd->clear();
    			lcd->setCursor(0,0);
    			lcd->write("light value ");
    			lcd->setCursor(1,2);
    			lcd->write("in lux: ");
    			lcd->write(litdata);

    			sleep(3);
    			lcd->clear();
    			x = 0;
    		}
    	}
    }
    delete button;
    delete temp;
    delete knob;
    delete light;
    return 0;
}

