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
 *
 * Use a platform with Analog and GPIO capabilities
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

// Define the following if using a Grove Pi Shield
#define USING_GROVE_PI_SHIELD
using namespace mraa;
using namespace std;

/*
 * Continuously acquire the average sound level over a predefined interval and map
 * it to the led bar
 */

int main(int argc, char **argv) {
  string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
      "you are running it on an unrecognized platform. "
    "You may need to modify the MRAA/UPM initialization code to "
    "ensure it works properly on your platform.\n\n";

  // Threshold context for the sound sensor
  thresholdContext ctx;

  uint16_t buffer[50];  // sound samples buffer
  uint8_t barLevel = 0;
  int microphonePin, ledBarDataPin, ledBarClockPin;

  // check which board we are running on
  Platform platform = getPlatformType();
  switch (platform) {
    case INTEL_UP2:
#ifdef USING_GROVE_PI_SHIELD //Needs offset by 512
      microphonePin = 0 + 512; // A0
      ledBarDataPin = 2 + 512;  // D2
      ledBarClockPin = 3 + 512; ///D3
      break;
#else
      cerr << "Not using Grove, provide your pinout" << endl;
      return -1;
#endif
    default:
          cerr << unknownPlatformMessage;
  }
#ifdef USING_GROVE_PI_SHIELD
  addSubplatform(GROVEPI, "0");
#endif
  // check if running as root
  int euid = geteuid();
  if (euid) {
    cerr << "This project uses Mraa I/O operations, but you're not running as 'root'.\n"
        "The IO operations below might fail.\n"
        "See the project's Readme for more info.\n\n";
  }

  // Sound sensor connected to analog in
  upm::Microphone* mic = new upm::Microphone(microphonePin);

  // Led Bar connected to digital out
  upm::GroveLEDBar* bar = new upm::GroveLEDBar(ledBarDataPin, ledBarClockPin);

  // Simple error checking
  if ((mic == NULL) || (bar == NULL)) {
    std::cerr << "Can't create all objects, exiting" << std::endl;
    return ERROR_UNSPECIFIED;
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

  return SUCCESS;
}
