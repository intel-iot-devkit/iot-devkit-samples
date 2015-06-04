/*
 * Author: Sisinty Sasmita Patra <sisinty.s.patra@intel.com>
 * Copyright (c) 2015 Intel Corporation.
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
 *   
 */
#include "jhd1313m1.h"   //lcd display
#include "grove.h"     
#include "adxl345.h"     //3-Axis digital accelerometer
#include "buzzer.h"	 //buzzer
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
    upm::GroveTemp* temp = new upm::GroveTemp(1); //pin A1
   
    upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62); //pin I2C

    for (int i=0; i < 5; i++) {
    	celsius = temp->value();
    }

    string tdata = static_cast<ostringstream*>( &(ostringstream() << celsius))-> str();
    cout << "tdata: " << tdata << endl;
    
    while(button->value() == 0)
    {
    	lcd->setCursor(0,0);
    	lcd->write("welcome ");
    	lcd->setCursor(1,2);
    	lcd->write("press button!");

    	usleep(100000); //sleep for 1 second
    }
	
    if(button->value() == 1)
    {
	lcd->clear();
	lcd->setCursor(0,0);
	lcd->write("Temperature in ");
	lcd->setCursor(1,2);
	lcd->write("celsius: ");
	lcd->write(tdata);

	sleep(1);
	if(button->value() == 0)
		break;
    }

    delete temp;
    return 0;
}

