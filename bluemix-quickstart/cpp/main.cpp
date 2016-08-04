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

const char * topic = "iot-2/evt/status/fmt/json";
const char * clientID = strcat(
        strcpy(new char[50], "d:quickstart:iotquick-edison:"), DEVICE_ID);
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

/*
 * Update the temperature values and send the values to IBM Bluemix
 * - blink the led to show the temperature was measured and data updated
 */
void temperature_update(upm::GroveTemp* temperature_sensor,
        upm::GroveLed* led) {
    // other helper variables
    int temperature; // temperature sensor value in degrees Celsius
    std::stringstream row_1; // temperature

    // update temperature values
    temperature = temperature_sensor->value();
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
    led->on();
    usleep(50000);
    led->off();
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

int main() {
    // check that we are running on Galileo or Edison
    mraa::Platform platform = mraa::getPlatformType();
    if ((platform != mraa::INTEL_GALILEO_GEN1)
            && (platform != mraa::INTEL_GALILEO_GEN2)
            && (platform != mraa::INTEL_EDISON_FAB_C)) {
        std::cerr << "Unsupported platform, exiting" << std::endl;
        return mraa::ERROR_INVALID_PLATFORM;
    }

    // create the MQTT client
    int rc = 0;
    rc = MQTTClient_create(&client, const_cast<char *>(host),
            const_cast<char *>(clientID), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        std::cerr << "Failed to create MQTT client, exiting" << std::endl;
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
        std::cerr << "Failed to connect MQTT client, exiting" << std::endl;
        exit(rc);
    }

    // led connected to D3 (digital out)
    upm::GroveLed* led = new upm::GroveLed(3);

    // temperature sensor connected to A0 (analog in)
    upm::GroveTemp* temp_sensor = new upm::GroveTemp(0);

    // simple error checking
    if ((led == NULL) || (temp_sensor == NULL)) {
        std::cerr << "Can't create all objects, exiting" << std::endl;
        return mraa::ERROR_UNSPECIFIED;
    }

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
