/*
 * Author: Ivan De Cesaris <ivan.de.cesaris@intel.com>
 * Author: Petre Eftime <petre.p.eftime@intel.com>
 *
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
 * @file
 * @ingroup grove
 * @brief Display temperature on LCD
 *
 * This project template includes the starting code needed
 * to use the various sensors that come with the Grove Starter Kit.
 * Update the temperature values and reflect the changes on the LCD.
 *
 * @hardware - Grove LED (GroveLed) connected to the Grove Base Shield Port D3
 * - Grove Button (GroveButton) connected to the Grove Base Shield Port D4
 * - Grove Temperature Sensor (GroveTemp) connected to the Grove Base Shield Port A0
 * - Grove LCD (Jhd1313m1) connected to any I2C on the Grove Base Shield
 *
 *  TODO use a platfrom with I2C, Analog and GPIO capabilities
 */
package iotdk.example;

import upm_grove.GroveButton;
import upm_grove.GroveLed;
import upm_grove.GroveTemp;
import upm_i2clcd.Jhd1313m1;
import mraa.mraa;
import mraa.Platform;

public class DisplayTemperatureOnLCD {

    // minimum and maximum temperatures registered, the initial values will be
    // replaced after the first read
    static int min_temperature = Integer.MAX_VALUE;
    static int max_temperature = Integer.MIN_VALUE;


    /*
     * Update the temperature values and reflect the changes on the LCD
     * - change LCD backlight color based on the measured temperature,
     *   a cooler color for low temperatures, a warmer one for high temperatures
     * - display current temperature
     * - record and display MIN and MAX temperatures
     * - reset MIN and MAX values if the button is being pushed
     * - blink the led to show the temperature was measured and data updated
     */
    static void temperature_update(GroveTemp temp, GroveButton button, GroveLed led,
            Jhd1313m1 lcd) {

        // the temperature range in degrees Celsius,
        // adapt to your room temperature for a nicer effect!
        final int TEMPERATURE_RANGE_MIN_VAL = 18;
        final int TEMPERATURE_RANGE_MAX_VAL = 31;

        // other helper variables
        int temperature; // temperature sensor value in degrees Celsius
        float fade; // fade value [0.0 .. 1.0]
        short r, g, b; // resulting LCD backlight color components [0 .. 255]

        // update the min and max temperature values, reset them if the button is
        // being pushed
        temperature = temp.value();
        if (button.value() == 1) {
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
        lcd.setCursor(0,0);
        lcd.write(String.format("Temp %d    ", temperature));
        lcd.setCursor(1,0);
        lcd.write(String.format("Min %d Max %d    ", min_temperature,
                max_temperature));

        // set the fade value depending on where we are in the temperature range
        if (temperature <= TEMPERATURE_RANGE_MIN_VAL) {
            fade = 0.0f;
        } else if (temperature >= TEMPERATURE_RANGE_MAX_VAL) {
            fade = 1.0f;
        } else {
            fade = (float)(temperature - TEMPERATURE_RANGE_MIN_VAL) /
                    (TEMPERATURE_RANGE_MAX_VAL - TEMPERATURE_RANGE_MIN_VAL);
        }

        // fade the color components separately
        r = (short)(255 * fade);
        g = (short)(64 * fade);
        b = (short)(255 * (1 - fade));

        // blink the led for 50 ms to show the temperature was actually sampled
        led.on();
        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            System.err.println("Sleep interrupted: " + e.toString());
        }
        led.off();

        // apply the calculated result
        lcd.setColor(r, g, b);
    }

    public static void main(String[] args) {
        // button connected to D4 (digital in)
        GroveButton button = new GroveButton(4);

        // led connected to D3 (digital out)
        GroveLed led = new GroveLed(3);

        // temperature sensor connected to A0 (analog in)
        GroveTemp temp = new GroveTemp(0);

        // LCD connected to the default I2C bus
        Jhd1313m1 lcd = new Jhd1313m1(0);

        // loop forever updating the temperature values every second
        while (true) {
            temperature_update(temp, button, led, lcd);
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.err.println("Sleep interrupted: " + e.toString());
            }
        }

    }

}
