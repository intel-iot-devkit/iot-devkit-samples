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
 * This class is a base class used to connect clients to the IBM* Bluemix
 *
 * * Other names and brands may be claimed as the property of others.
 */
#include "BluemixFDClientBase.hpp"

#include "string.h"

#include <iostream>

using namespace std;

/*
 * Called when a message has been successfully delivered. Not called with QoS0.
 * @param context the application specific context.
 * @param dt the delivery token sent with the message.
 */
void Client_base::delivery_complete(void * context, MQTTClient_deliveryToken dt)
{
	printf("Publishing of message with token %d confirmed\n", dt);
}

/*
 * Called when the server connection has been lost.
 * @param context the application specific context.
 * @param cause the reason of the disconnect.
 */
void Client_base::connection_lost(void * context, char* cause)
{
	printf("App client lost Connection\n");
	Client_base * client = static_cast<Client_base *>(context);
	if (client != NULL) {
		client->connected = false;
	}
}

/*
 * Send a command to a specified topic.
 * @param payload the message payload.
 * @param payloadlen the message payload length.
 * @param topic the topic for that message.
 * @param qos the message qos.
 */
int Client_base::send_command(
		char * payload, int payloadlen, char * topic, int qos)
{
	if (connected && payloadlen > 0) {
		int retained = 0;
		MQTTClient_deliveryToken dt;
		int rc = MQTTClient_publish(client, topic,
			payloadlen, payload, qos, retained, &dt);
		if (rc == MQTTCLIENT_SUCCESS) {
			printf("Waiting for message with token %d to be published...\n", dt);
			rc = MQTTClient_waitForCompletion(client, dt, 1000);
			if (rc == MQTTCLIENT_SUCCESS) {
				printf("Message with token %d published\n", dt);
			} else {
				fprintf(stderr, "Failed to publish message with token %d", dt);
				return MQTTCLIENT_FAILURE;
			}
		} else {
			fprintf(stderr, "Failed to publish message with token %d", dt);
			return MQTTCLIENT_FAILURE;
		}
		return MQTTCLIENT_SUCCESS;
	}
	return MQTTCLIENT_FAILURE;
}

/*
 * Establish a client connection to the server.
 * @param host the hostname or IP address.
 * @param username the user name.
 * @param password the password.
 * @param clientID the client id for the connection.
 * @param cl A callback to be called when the client has disconnected,
 * or NULL.
 * @param ma A callback to be called when a message has been arrived,
 * or NULL.
 * @param dc A callback to be called when the message has been delivered,
 * or NULL.
 */
int Client_base::connect(char * host, char * username, char * password,
		char * clientID, MQTTClient_connectionLost* cl,
		MQTTClient_messageArrived* ma, MQTTClient_deliveryComplete* dc)
{
	// disconnect if already connected
	if (client != NULL) {
		disconnect();
	}
	// create the MQTT client
	int rc = 0;
	rc = MQTTClient_create(&client, host,
			clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to create MQTT client");
		return rc;
	}

	// setup call backs
	rc = MQTTClient_setCallbacks(client, this, cl, ma, dc);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to set call backs");
		return rc;
	}

	MQTTClient_connectOptions data = MQTTClient_connectOptions_initializer;
	data.username = username;
	data.password = password;
	// connect the client to the server
	rc = MQTTClient_connect(client, &data);
	if (rc != MQTTCLIENT_SUCCESS) {
		fprintf(stderr, "Failed to connect MQTT client");
		return rc;
	}
	connected = true;
	return rc;
}

/*
 * Disconnect and destroy the application client.
 */
void Client_base::disconnect()
{
	if (client != NULL) {
		connected = false;
		MQTTClient_disconnect(client, 100);
		MQTTClient_destroy(&client);
		client = NULL;
	}
}
