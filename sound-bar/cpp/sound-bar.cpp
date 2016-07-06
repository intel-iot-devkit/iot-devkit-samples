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
#include <mic.hpp>
#include <groveledbar.hpp>
#include <grovecircularled.hpp>

/**
 * This example show how to use a Led Bar to show ambient sound level.
 *
 * Hardware Sensors used:\n
 * Grove Sound Sensor connected to the Grove Base Shield Port A0\n
 * Grove Led Bar connected to the Grove Base Shield Port D2
 */

#define SOUND_MIN    50
#define SOUND_MAX    200
#define LED_BAR_MIN  0
#define LED_BAR_MAX  10
#define SAMPLE_FREQUENCE 1 // microsecond
#define SAMPLE_WINDOW    50
#define SAMPLE_THRESHOLD 80

#define SOUND_RANGE (SOUND_MAX - SOUND_MIN)
#define LED_BAR_RANGE (LED_BAR_MAX - LED_BAR_MIN)

/*
 * Continuously acquire the average sound level over a predefined interval and map
 * it to the led bar
 */

int main(int argc, char **argv) {

  // Check that we are running on Galileo or Edison
  mraa::Platform platform = mraa::getPlatformType();
  if ((platform != mraa::INTEL_GALILEO_GEN1)
      && (platform != mraa::INTEL_GALILEO_GEN2)
      && (platform != mraa::INTEL_EDISON_FAB_C)) {
    std::cerr << "Unsupported platform, exiting" << std::endl;
    return mraa::ERROR_INVALID_PLATFORM;
  }

  // Threshold context for the sound sensor
  thresholdContext ctx;

  uint16_t buffer[50];  // sound samples buffer
  uint8_t barLevel = 0;

  // Sound sensor connected to A0 (analog in)
  upm::Microphone* mic = new upm::Microphone(0);

  // Led Bar connected to D2 (digital out)
  upm::GroveLEDBar* bar = new upm::GroveLEDBar(2, 3);

  // Simple error checking
  if ((mic == NULL) || (bar == NULL)) {
    std::cerr << "Can't create all objects, exiting" << std::endl;
    return mraa::ERROR_UNSPECIFIED;
  }

  // Initialize the threshold context
  ctx.averageReading = 0;
  ctx.runningAverage = 0;
  ctx.averagedOver = 10;

  // Repeatedly acquire the average sound level and map it to led bar range

  for (;;) {

    int len = mic->getSampledWindow(1, SAMPLE_WINDOW, buffer);

    if (len) {

      mic->findThreshold(&ctx, SAMPLE_THRESHOLD, buffer, len);

      barLevel = (ctx.runningAverage - SOUND_MIN) * LED_BAR_RANGE / SOUND_RANGE
          + LED_BAR_MIN;

      bar->setBarLevel(barLevel);
    }
  }

  return mraa::SUCCESS;
}
