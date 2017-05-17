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

#ifndef __CLOUD_CLIENT_BASE_H
#define __CLOUD_CLIENT_BASE_H 1

extern "C" {
#include "MQTTClient.h"
}

#define MAX_CHAR 128

class Client_base
{
private:
	bool connected;
protected:
	MQTTClient client;

	/*
	 * Called when a message has been successfully delivered. Not called with QoS0.
	 * @param context the application specific context.
	 * @param dt the delivery token sent with the message.
	 */
	static void delivery_complete(void * context, MQTTClient_deliveryToken dt);

	/*
	 * Called when the server connection has been lost.
	 * @param context the application specific context.
	 * @param cause the reason of the disconnect.
	 */
	static void connection_lost(void * context, char* cause);

	/*
	 * Send a command to a specified topic.
	 * @param payload the message payload.
	 * @param payloadlen the message payload length.
	 * @param topic the topic for that message.
	 * @param qos the message qos.
	 */
	int send_command(char * payload, int payloadlen, char * topic, int qos);
public:
	Client_base() : connected(false), client(NULL) {};
	virtual ~Client_base() { disconnect(); }

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
	int connect(char * host, char * username, char * password,
			char * clientID, MQTTClient_connectionLost* cl,
			MQTTClient_messageArrived* ma, MQTTClient_deliveryComplete* dc);

	/*
	 * Disconnect and destroy the application client.
	 */
	void disconnect();

	/*
	 * Return true if the application client is connected, otherwise it
	 * will return false.
	 */
	inline bool is_connected() {
		return connected;
	}
};

#endif // __CLOUD_CLIENT_BASE_H
