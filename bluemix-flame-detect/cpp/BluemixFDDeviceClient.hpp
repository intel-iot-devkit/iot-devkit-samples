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

#ifndef __CLOUD_DEVICE_CLIENT_H
#define __CLOUD_DEVICE_CLIENT_H 1

extern "C" {
#include "MQTTClient.h"
}

#include "BluemixFDClientBase.hpp"

class Device_client : public Client_base
{
private:
	char host[MAX_CHAR];
	char topic[MAX_CHAR];
	char cmd[MAX_CHAR];
	char clientID[MAX_CHAR];
	int (*fire_alert_callback)(void);

	/*
	 * Called when a message has been arrived.
	 * @param context the application specific context.
	 * @param topicName the topic name of the message.
	 * @param topicLen the topic name length.
	 * @param message the message that has been arrived.
	 */
	static int message_arrived(void* context, char* topicName, int topicLen,
			MQTTClient_message* message);
public:
	Device_client(int (* callback)(void));
	~Device_client() {}

	/*
	 * Connect the device client with the server.
	 */
	int connect();

	/*
	 * Send message to the application if a fire has been detected.
	 * @param fireDetected Specifies if a fire has been detected.
	 */
	int send_fire_detected(bool fireDetected);

	/*
	 * Subscribe to commands send from the application client.
	 */
	int subscribe_to_cmd();
};

#endif // __CLOUD_DEVICE_CLIENT_H
