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

/*
 * This class is used to connect a application client to the IBM* Bluemix
 * listening for events from the device client. The application client will
 * respond, by sending a command, depending on the message received from the
 * device client.
 *
 * * Other names and brands may be claimed as the property of others.
 */
#include "BluemixFDAppClient.hpp"
#include "BluemixFDCredentials.hpp"

#include "string.h"

#include <iostream>

using namespace std;

/*
 * Initialize connection settings in the constructor
 */
App_client::App_client()
{
	int res = 0;
	// Initialize the host URL
	// tcp://{organization id}.messaging.internetofthings.ibmcloud.com:1883
	// Use 443 for secure connection
	res = snprintf(host, MAX_CHAR,
			"tcp://%s.messaging.internetofthings.ibmcloud.com:%s",
			BLUEMIX_FD_ORGANIZATION_ID, BLUEMIX_FD_PORT_NUMBER);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize app client host");
	}
	// Initialize the topic (+ -> means any)
	// iot-2/type/{device type}/id/{device id}/evt/+/fmt/json
	res = snprintf(topic, MAX_CHAR, "iot-2/type/%s/id/%s/evt/+/fmt/json",
			BLUEMIX_FD_DEVICE_TYPE, BLUEMIX_FD_DEVICE_ID);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize app client topic");
	}
	// Initialize the command topic published to the device
	// iot-2/type/{device type}/id/{device id}/cmd/{command id}/fmt/json
	res = snprintf(app_cmd, MAX_CHAR, "iot-2/type/%s/id/%s/cmd/%s/fmt/json",
			BLUEMIX_FD_DEVICE_TYPE, BLUEMIX_FD_DEVICE_ID,
			BLUEMIX_FD_FIRE_ALERT_CMD_ID);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize app client app_cmd");
	}
	// Initialize client id a:{organization id}:{device type}:{device id}
	res = snprintf(app_clientID, MAX_CHAR, "a:%s:myapp",
			BLUEMIX_FD_ORGANIZATION_ID);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize app client app_clientID");
	}
}

/*
 * Called when a message has been arrived.
 * @param context the application specific context.
 * @param topicName the topic name of the message.
 * @param topicLen the topic name length.
 * @param message the message that has been arrived.
 */
int App_client::message_arrived(void* context, char* topicName, int topicLen,
		MQTTClient_message* message)
{
	// received message from a device
	printf("App client received message...\n");
	App_client * app = static_cast<App_client *>(context);
	if (app != NULL) {
		char * payload = (char *) message->payload;
		string payloadStr(payload);
		if (message->payloadlen > 0) {
			// Send fire alert if the message contains "fireDetected: 1"
			if (payloadStr.find("\"fireDetected\": \"1\"") != string::npos) {
				app->send_fire_alert(true);
			} else {
				app->send_fire_alert(false);
			}
		}
	}
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	MQTTClient_yield();
	return 1;
}

/*
 * This function is sending an command to turn on/off the fire alert.
 * <b>Note:</b> With IBM Quickstart the command will not be send. An API-key
 * and authentication token is required to send the command to the device client
 */
int App_client::send_fire_alert(bool on)
{
	// sending commands is not supported with IBM Quickstart, this requires
	// an account
	if (is_connected() && BLUEMIX_FD_APP_USERNAME != NULL &&
			BLUEMIX_FD_APP_PASSWORD != NULL) {
		char payload[80];
		sprintf(payload, "{ \"fireAlertTriggered\": \"%d\" }", on);
		int payloadlen = strlen(payload);
		return Client_base::send_command(
				payload, payloadlen, app_cmd, MQTT_DEFAULT_QOS);
	}
	return MQTTCLIENT_FAILURE;
}

/*
 * Connect the application client with the server
 */
int App_client::connect()
{
	// connect the MQTT client
	return Client_base::connect(host, (char *) BLUEMIX_FD_APP_USERNAME,
			(char *) BLUEMIX_FD_APP_PASSWORD, app_clientID,
			Client_base::connection_lost,
			App_client::message_arrived, Client_base::delivery_complete);
}

/*
 * Subscribe to events send from the device client
 */
int App_client::subscribe_to_event()
{
	int rc = MQTTCLIENT_FAILURE;
	// check if the application client is connected
	if (is_connected()) {
		rc = MQTTClient_subscribe(client, topic, MQTT_DEFAULT_QOS);
		if (rc != MQTTCLIENT_SUCCESS) {
			disconnect();
			fprintf(stderr, "Failed to subscribe MQTT to event");
		}
	}
	return rc;
}
