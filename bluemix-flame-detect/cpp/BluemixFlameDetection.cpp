/*
 * Author: Marc Ernst <marc.ernst@intel.com>
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
 * Demonstration of a IBM* Bluemix device client and application client communicating
 * with each other.
 * This project template is using the flame sensor to simulate fire detection.
 * The result is send through the device client to the IBM* Bluemix. The application
 * listening for the device topic will be notified when the message has been sent.
 * Depending on the result, the application will send a command to the device client
 * to turn on/off the fire alert. The fire alert will be simulated by enabling the
 * Grove Buzzer for some seconds.\n\n
 *
 * Note: Publishing and subscribing to commands requires an account and is not
 * available with IBM* Quickstart.\n\n
 *
 * First steps:\n
 * Go to https://quickstart.internetofthings.ibmcloud.com/#/ and enter
 * the device id used in the credentials header file.\n
 * Run the example and check the results at:\n
 * https://quickstart.internetofthings.ibmcloud.com/#/device/{DEVICE ID}/sensor/
 * \n
 * \n* Other names and brands may be claimed as the property of others.
 *
 * Hardware Sensors used:\n
 * Grove Buzzer (GroveLed)\n
 * connected to the Grove Base Shield Port D5\n
 * Flame Sensor (YG1006)\n
 * connected to the Grove Base Shield Port D3\n
 *
 */

#include "mraa.hpp"
#include <iostream>
#include <yg1006.hpp>
#include <unistd.h>
#include <buzzer.hpp>
#include <buzzer_tones.hpp>

#include "BluemixFDAppClient.hpp"
#include "BluemixFDDeviceClient.hpp"
using namespace std;

upm::Buzzer * buzzer;

/*
 * This function is using the flame sensor to detect a fire and calls the
 * device client to send the message to the application
 * @param device_client A handle of the device client
 * @param app_client A handle of the application client
 */
int detect_flame(Device_client * device_client, App_client * app_client)
{
	upm::YG1006 flameSensor = upm::YG1006(3);

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
		bool flameDetected = flameSensor.flameDetected();
		if (flameDetected) {
			printf("Flame or similar light source detected!\n");
		} else {
			printf("No flame detected.\n");
		}
		// send the result through the device client to the application
		device_client->send_fire_detected(flameDetected);
		sleep(3);
	}
	return mraa::SUCCESS;
}

/*
 * This function is called when a fire alert has been triggered
 */
int fire_alert() {
	printf("Fire alert called\n");
	if (buzzer == NULL) {
		fprintf(stderr, "Buzzer not initialized");
		return mraa::ERROR_INVALID_HANDLE;
	}
	// set the volume
	buzzer->setVolume(1.0);
	// fire alert
	buzzer->playSound(BUZZER_MI, 400000);
	return mraa::SUCCESS;
}

int main()
{
	// check that we are running on Galileo or Edison
	mraa::Platform platform = mraa::getPlatformType();
	if ((platform != mraa::INTEL_GALILEO_GEN1) &&
			(platform != mraa::INTEL_GALILEO_GEN2) &&
			(platform != mraa::INTEL_EDISON_FAB_C)) {
		fprintf(stderr, "Unsupported platform, exiting");
		return mraa::ERROR_INVALID_PLATFORM;
	}

	// initialize the buzzer
	buzzer = new upm::Buzzer(5);
	if (buzzer == NULL) {
		fprintf(stderr, "Buzzer not initialized");
		return mraa::ERROR_INVALID_HANDLE;
	}

	// initialize the device client
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
	detect_flame(&device_client, &app_client);
	// cleanup the buzzer
	delete buzzer;
	return mraa::SUCCESS;
}
