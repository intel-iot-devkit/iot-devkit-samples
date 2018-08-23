/*
 * Author: Marc Ernst <marc.ernst@intel.com>
 * Copyright (c) 2015 - 2017 Intel Corporation.
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
 * Demonstration of a IBM* Bluemix device client and application client communicating
 * with each other.
 * This project template is using the flame sensor to simulate fire detection.
 * The result is send through the device client to the IBM* Bluemix. The application
 * listening for the device topic will be notified when the message has been sent.
 * Depending on the result, the application will send a command to the device client
 * to turn on/off the fire alert. The fire alert will be simulated by enabling the
 * Grove Buzzer for some seconds.
 * 
 * Note: Publishing and subscribing to commands requires an account and is not
 * available with IBM* Quickstart.
 *
 * For instructions on how to setup the cloud service for this sample, see:
 * https://github.com/intel-iot-devkit/iot-devkit-samples/blob/iss2018-update1/bluemix-quickstart/README.md
 */

#include "mraa.hpp"
#include <iostream>
#include <yg1006.hpp>
#include <unistd.h>
#include <buzzer.hpp>
#include <buzzer_tones.h>

#include "BluemixFDAppClient.hpp"
#include "BluemixFDDeviceClient.hpp"
using namespace std;
using namespace mraa;

// define the following if not using the sensor & buzzer
//#define SIMULATE_DEVICES


#ifndef SIMULATE_DEVICES
    int dPin;
    int pwmPin;
#endif

// Define the following if using a Grove Pi Shield for Up Squared board
#define USING_GROVE_PI_SHIELD

upm::Buzzer * buzzer;

/*
 * This function is using the flame sensor to detect a fire and calls the
 * device client to send the message to the application
 * @param device_client A handle of the device client
 * @param app_client A handle of the application client
 */
int detect_flame(Device_client * device_client, App_client * app_client)
{

#ifndef SIMULATE_DEVICES
    upm::YG1006 flameSensor(dPin);
#endif
    /* Code in this loop will run repeatedly
     */
    for (;;) {
        // check if the device client is connected
        if (device_client == NULL || ! device_client->is_connected()) {
            app_client->disconnect();
            fprintf(stderr, "Device client disconnected, exiting");
            break;
        }
        // check if the application client is connected
        if (app_client == NULL || ! app_client->is_connected()) {
            device_client->disconnect();
            fprintf(stderr, "Application client disconnected, exiting");
            break;
        }

        // check for flames or similar light sources
        bool flameDetected = 0;
#ifndef SIMULATE_DEVICES
        flameDetected = flameSensor.flameDetected();
#endif
        if (flameDetected) {
            printf("Flame or similar light source detected!\n");
        } else {
            printf("No flame detected.\n");
        }
        // send the result through the device client to the application
        device_client->send_fire_detected(flameDetected);
        sleep(3);
    }
    return SUCCESS;
}

/*
 * This function is called when a fire alert has been triggered
 */
int fire_alert() {
    printf("Fire alert called\n");
#ifndef SIMULATE_DEVICES
    if (buzzer == NULL) {
        fprintf(stderr, "Buzzer not initialized");
        return ERROR_INVALID_HANDLE;
    }
    // set the volume
    buzzer->setVolume(1.0);
    // fire alert
    buzzer->playSound(BUZZER_MI, 400000);
#endif
    return SUCCESS;
}

// leave warning/error message in console and wait for user to press Enter
void consoleMessage(const string& str)
{
    cerr << str << endl;
    sleep(10);
}

// check if running as root
void checkRoot(void)
{
    int euid = geteuid();
    if (euid) {
        consoleMessage("This project uses Mraa I/O operations that require\n"
            "'root' privileges, but you are running as non - root user.\n"
            "Passwordless keys(RSA key pairs) are recommended \n"
            "to securely connect to your target with root privileges. \n"
            "See the project's Readme for more info.\n\n");
    }
    return;
}

// set pin values depending on the current board (platform)
void initPlatform(int& dPin, int& pwmPin)
{
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
        case INTEL_UP2:
            dPin = 13;  	// digital in
            pwmPin = 33;  	// PWM
#ifdef USING_GROVE_PI_SHIELD
            dPin   = 4 + 512; // D4 Connector (512 offset needed for the shield)
            pwmPin = 5 + 512; // D5 works as PWM on Grove PI Shield 
            break;
#endif
        default:
            string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
                "you are running it on an unrecognized platform. "
                "You may need to modify the MRAA/UPM initialization code to "
                "ensure it works properly on your platform.\n";
            consoleMessage(unknownPlatformMessage);
    }
    return;
}

int main()
{
    // check if running as root
    checkRoot();

#ifndef SIMULATE_DEVICES
    initPlatform(dPin, pwmPin);
#ifdef USING_GROVE_PI_SHIELD
    addSubplatform(GROVEPI, "0");
#endif

    // initialize the buzzer
    buzzer = new upm::Buzzer(pwmPin);
    if (buzzer == NULL) {
        fprintf(stderr, "Buzzer not initialized");
        return ERROR_INVALID_HANDLE;
    }
#endif
    // initialize the device client
    cout << "Connecting to MQTT client ..." << endl;

    Device_client device_client = Device_client(fire_alert);
    int rc = device_client.connect();
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr, "Could not connect device client to server, exiting");
        return rc;
    }
    // subscribe to commands is not available with IBM Quickstart,
    // this requires an account
    rc = device_client.subscribe_to_cmd();
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr,
        "Device client could not subscribe to command, account required");
    }

    // initialize the application client
    App_client app_client = App_client();
    rc = app_client.connect();
    if (rc != MQTTCLIENT_SUCCESS) {
        fprintf(stderr,
        "Could not connect application client to server, exiting");
        return rc;
    }
    // subscribe to events send from the device
    rc = app_client.subscribe_to_event();
    if (rc != MQTTCLIENT_SUCCESS) {
        return rc;
    }

    // start flame detection
#ifndef SIMULATE_DEVICES
    detect_flame(&device_client, &app_client);
#endif
    // cleanup the buzzer
    delete buzzer;
    return SUCCESS;
}
