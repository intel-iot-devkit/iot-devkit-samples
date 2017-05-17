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

#ifndef __CLOUD_CREDENTIALS_H
#define __CLOUD_CREDENTIALS_H 1

#include <unistd.h>

// Set to 1 if you want to ensure that the message has been published
// QoS0 --> Message will be delivered zero or once
// QoS1 --> Message will be delivered at least once
// QoS2 --> Message will be delivered exactly once
#define MQTT_DEFAULT_QOS 0

// Credentials used to connect the application and device client
#define BLUEMIX_FD_DEVICE_ID "edison"
#define BLUEMIX_FD_DEVICE_TYPE "iotquick-edison"
#define BLUEMIX_FD_ORGANIZATION_ID "quickstart"
// Fire alert command id
#define BLUEMIX_FD_FIRE_ALERT_CMD_ID "fireAlert"

// Port number to connect to - use 443 for secure connection
#define BLUEMIX_FD_PORT_NUMBER "1883"

/*
 * To authenticate with a token, set the username to "use-token-auth" and the
 * password to the token which you get when you register your device in IBM* Bluemix.
 */
#define BLUEMIX_FD_CLIENT_USERNAME NULL
#define BLUEMIX_FD_CLIENT_PASSWORD NULL

/*
 * To authenticate with a token, sign in to  the  IBM* Internet of Things Foundation
 * and create an API-Key for your organization. The API-key in form of
 * a-org_id-key will be the username. The password will be the authentication token.
 */
#define BLUEMIX_FD_APP_USERNAME NULL
#define BLUEMIX_FD_APP_PASSWORD NULL

/*
 * * Other names and brands may be claimed as the property of others.
 */

#endif /* __CLOUD_CREDENTIALS_H */
