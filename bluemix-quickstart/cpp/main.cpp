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
 * For instructions on how to setup the cloud service for this sample, see:
 * https://github.com/intel-iot-devkit/iot-devkit-samples/blob/iss2018-update1/bluemix-quickstart/README.md
 */


extern "C" {
    #include "MQTTClient.h"
    }
    
#include <mraa.hpp>
#include "stdlib.h"

#include <grove.hpp>
#include <climits>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "credentials.h"

using namespace std;
using namespace mraa;
    
// define the following if not using a board with temp sensor and LED
//#define SIMULATE_DEVICES

// Define the following if using a Grove Pi Shield for Up Squared board
#define USING_GROVE_PI_SHIELD
    
const char * topic = "iot-2/evt/status/fmt/json";
const char * clientID = strcat(
        strcpy(new char[50], "d:quickstart:iotquick-my-iot-board:"), DEVICE_ID);
// To authenticate with a token, set the username to "use-token-auth" and the
// password to the token which you get when you register your device in IBM Bluemix.
const char * username = NULL;
const char * password = NULL;
const char * host = "tcp://quickstart.messaging.internetofthings.ibmcloud.com:1883";

// Set to 1 if you want to ensure that the message has been published
// QoS0 --> Message will be delivered zero or once
// QoS1 --> Message will be delivered at least once
// QoS2 --> Message will be delivered exactly once
#define MQTT_DEFAULT_QOS 0

MQTTClient client;
MQTTClient_deliveryToken dt = 0;


// leave warning/error message in console and wait for user to press Enter
void consoleMessage(const string& str)
{
    cerr << str << endl;
    sleep(10);
}

/*
 * Update the temperature values and send the values to IBM Bluemix
 * - blink the led to show the temperature was measured and data updated
 */
void temperature_update(upm::GroveTemp* temperature_sensor,
        upm::GroveLed* led) {
    // other helper variables
    int temperature = 0; // temperature sensor value in degrees Celsius
    std::stringstream row_1; // temperature

    // update temperature values
#ifndef SIMULATE_DEVICES
    temperature = temperature_sensor->value();
#else
    temperature++;
#endif
    // convert the message into JSON format
    row_1 << "{ \"temp\": \"" << temperature << "\" }";

    // send message to IBM Bluemix
    char payload[80];
    sprintf(payload, "{ \"temp\": \"%d\" }", temperature);
    int payloadlen = strlen(payload);

    int retained = 0;
    int rc = MQTTClient_publish(client, const_cast<char *>(topic), payloadlen,
            const_cast<char *>(payload), MQTT_DEFAULT_QOS, retained, &dt);
    if (rc == MQTTCLIENT_SUCCESS) {
        printf("Waiting for message with token %d to be published...\n", dt);
        rc = MQTTClient_waitForCompletion(client, dt, 1000);
        if (rc == MQTTCLIENT_SUCCESS) {
            printf("Message with token %d published\n", dt);
        } else {
            std::cerr << "Failed to publish message with token " << dt
                    << std::endl;
        }
    } else {
        std::cerr << "Failed to publish message with token " << dt << std::endl;
    }

    dt++;
    // blink the led for 50 ms to show the temperature was actually sampled
#ifndef SIMULATE_DEVICES
    led->on();
#endif
    usleep(50000);
#ifndef SIMULATE_DEVICES
    led->off();
#endif
}
    
/*
 * This function is called when a message has been successfully published
 * to the server
 * <b>Note:</b>This function is not called when messages are
 * published at QoS0.
 * @param context A pointer to the <i>context</i> value originally passed to
 * MQTTClient_setCallbacks(), which contains any application-specific context.
 * @param dt The ::MQTTClient_deliveryToken associated with
 * the published message. Applications can check that all messages have been
 * correctly published by matching the delivery tokens returned from calls to
 * MQTTClient_publish() and MQTTClient_publishMessage() with the tokens passed
 * to this callback.
 */
void delivery_complete(void * context, MQTTClient_deliveryToken dt) {
    printf("Publishing of message with token %d confirmed\n", dt);
}

/*
 * This function will be called when the MQTT client lost the
 * connection to the server
 * @param context A pointer to the <i>context</i> value originally passed to
 * MQTTClient_setCallbacks(), which contains any application-specific context.
 * @param cause The reason for the disconnection.
 * Currently, <i>cause</i> is always set to NULL.
 */
void connection_lost(void * context, char* cause) {
    printf("Connection lost\n");
    exit(MQTTCLIENT_DISCONNECTED);
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
void initPlatform(int& dPin, int& aPin)
{
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_UP2:
        dPin = 3;  		// D3 Connector
        aPin = 2;  	    // A2 Connector
#ifdef USING_GROVE_PI_SHIELD
        dPin += 512;   // 512 offset needed for the shield
        aPin = 2 + 512; // A2 connector, 512 offset needed for the shield
        break;
#endif
    default:
        string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n\n";
        consoleMessage(unknownPlatformMessage);
    }
    return;
}
    
int main() {
    // check if running as root
    checkRoot();

#ifndef SIMULATE_DEVICES
    int dPin, aPin;
    initPlatform(dPin, aPin);
#ifdef USING_GROVE_PI_SHIELD
    addSubplatform(GROVEPI, "0");
#endif

#endif

    cout << "Connecting to MQTT client ..." << endl;

    // create the MQTT client
    int rc = 0;
    rc = MQTTClient_create(&client, const_cast<char *>(host),
            const_cast<char *>(clientID), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        consoleMessage("Failed to create MQTT client, exiting");
        exit(rc);
    }

    // setup call backs before connecting the client to the server
    MQTTClient_setCallbacks(client, NULL, &connection_lost, NULL,
            &delivery_complete);

    MQTTClient_connectOptions data = MQTTClient_connectOptions_initializer;
    data.username = const_cast<char *>(username);
    data.password = const_cast<char *>(password);
    // connect the client to the server
    rc = MQTTClient_connect(client, &data);
    if (rc != MQTTCLIENT_SUCCESS) {
        consoleMessage("Failed to connect MQTT client, exiting");
        exit(rc);
    }

    upm::GroveLed* led = NULL;
    upm::GroveTemp* temp_sensor = NULL;
#ifndef SIMULATE_DEVICES
    // led connected to Dx (digital out)
    led = new upm::GroveLed(dPin);

    // temperature sensor connected to Ax (analog in)
    temp_sensor= new upm::GroveTemp(aPin);

    // simple error checking
    if ((led == NULL) || (temp_sensor == NULL)) {
        consoleMessage("Can't create all objects, exiting");
        return mraa::ERROR_UNSPECIFIED;
    }
#endif
    // loop forever updating the temperature values every second
    for (;;) {
        temperature_update(temp_sensor, led);
        sleep(1);
    }

    printf("Stopping\n");

    int timeout = 100;
    MQTTClient_disconnect(client, timeout);
    MQTTClient_destroy(&client);
    return mraa::SUCCESS;
}
    
