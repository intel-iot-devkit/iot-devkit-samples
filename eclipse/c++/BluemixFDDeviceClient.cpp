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
 * This class is used to connect a device client to the IBM* Bluemix
 * listening for events from the application client.
 *
 * * Other names and brands may be claimed as the property of others.
 */

#include "BluemixFDDeviceClient.hpp"
#include "BluemixFDCredentials.hpp"

#include "string.h"

#include <iostream>

using namespace std;

/*
 * Initialize connection settings in the constructor
 */
Device_client::Device_client(int (* callback)(void)) :
	fire_alert_callback(callback)
{
	int res = -1;
	// Initialize the host URL
	// tcp://{organization id}.messaging.internetofthings.ibmcloud.com:1883
	// Use 443 for secure connection
	res = snprintf(host, MAX_CHAR,
			"tcp://%s.messaging.internetofthings.ibmcloud.com:%s",
			BLUEMIX_FD_ORGANIZATION_ID, BLUEMIX_FD_PORT_NUMBER);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize device client host");
	}
	// Initialize the topic iot-2/evt/status/fmt/json
	res = snprintf(topic, MAX_CHAR, "iot-2/evt/status/fmt/json");
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize device client topic");
	}
	// Initialize the command topic to subscribe to
	// iot-2/cmd/{command id}/fmt/json
	res = snprintf(cmd, MAX_CHAR, "iot-2/cmd/%s/fmt/json",
			BLUEMIX_FD_FIRE_ALERT_CMD_ID);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize device client cmd");
	}
	// Initialize client id d:{organization id}:{device type}:{device id}
	res = snprintf(clientID, MAX_CHAR, "d:%s:%s:%s",
			BLUEMIX_FD_ORGANIZATION_ID, BLUEMIX_FD_DEVICE_TYPE, BLUEMIX_FD_DEVICE_ID);
	if (res < 0 && res >= MAX_CHAR) {
		fprintf(stderr, "Failed to initialize device client clientID");
	}
}

/*
 * Called when a message has been arrived.
 * @param context the application specific context.
 * @param topicName the topic name of the message.
 * @param topicLen the topic name length.
 * @param message the message that has been arrived.
 */
int Device_client::message_arrived(void* context, char* topicName, int topicLen,
		MQTTClient_message* message)
{
	printf("Device client received Message...\n");
	if(Device_client * client = static_cast<Device_client *>(context)) {
		if (client->is_connected()) {
			char * payload = (char *) message->payload;
			string payloadStr(payload);
			if (message->payloadlen > 0) {
				// trigger fire alert
				if (payloadStr.find("\"fireAlertTriggered\": \"1\"")
						!= string::npos) {
					if (client->fire_alert_callback != NULL) {
						client->fire_alert_callback();
					}
				}
			}
		}
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/*
 * Connect the device client with the server
 */
int Device_client::connect()
{
	// connect the MQTT client
	return Client_base::connect(host, (char *) BLUEMIX_FD_CLIENT_USERNAME,
			(char *) BLUEMIX_FD_CLIENT_PASSWORD,
			clientID, Client_base::connection_lost,
			Device_client::message_arrived, Client_base::delivery_complete);
}

/*
 * Send message to the application if a fire has been detected
 * @param fireDetected Specifies if a fire has been detected
 */
int Device_client::send_fire_detected(bool fireDetected)
{
	// send message to IBM Bluemix
	if (is_connected()) {
		char payload[80];
		sprintf(payload, "{ \"fireDetected\": \"%d\" }", fireDetected);
		int payloadlen = strlen(payload);
		return Client_base::send_command(payload, payloadlen,
				topic, MQTT_DEFAULT_QOS);
	}
	return MQTTCLIENT_DISCONNECTED;
}

/*
 * Subscribe to commands send from the application client
 */
int Device_client::subscribe_to_cmd()
{
	int rc = MQTTCLIENT_FAILURE;
	// check if the device client is connected
	// subscribing to commands is not available with IBM Quickstart
	if (is_connected() && BLUEMIX_FD_CLIENT_USERNAME != NULL &&
			BLUEMIX_FD_CLIENT_PASSWORD != NULL) {
		rc = MQTTClient_subscribe(client, cmd, MQTT_DEFAULT_QOS);
		if (rc != MQTTCLIENT_SUCCESS) {
			fprintf(stderr, "Failed to subscribe MQTT to cmd");
		}
	}
	return rc;
}
