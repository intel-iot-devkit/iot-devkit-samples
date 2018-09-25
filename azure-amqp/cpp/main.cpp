/*
 * Microsoft Azure IoT SDKs
 * Copyright (c) Microsoft Corporation
 * All rights reserved.
 *
 * Author: Thomas Lyet <thomas.lyet@intel.com>
 * Copyright (c) 2016-2018 Intel Corporation.
 *
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the ""Software""), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * For instructions on how to setup the cloud service for this sample, see:
 * https://github.com/intel-iot-devkit/iot-devkit-samples/blob/iss2018-update1/azure-amqp/README.md
 */


#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "iothub_client_ll.h"
#include "iothub_message.h"
#include "iothubtransportamqp.h"

// define the following if not using a board with sensors
//#define SIMULATE_DEVICES

// Define the following if using a Grove Pi Shield for Up Squared board
#define USING_GROVE_PI_SHIELD

#include <iostream>

#include "certs.h"
#include "credentials.h"
#include <mraa.hpp>

#ifndef SIMULATE_DEVICES
#include <grove.hpp>
#endif

using namespace std;
using namespace mraa;

static int callbackCounter;
static bool g_continueRunning;
static char msgText[1024];
static char propText[1024];
#define MESSAGE_COUNT       5
#define DOWORK_LOOP_NUM     3

typedef struct EVENT_INSTANCE_TAG {
    IOTHUB_MESSAGE_HANDLE messageHandle;
    size_t messageTrackingId; // For tracking the messages within the user callback.
} EVENT_INSTANCE;

/**
 * Message call back function that will be called when an answer from
 * the Microsoft* Azure* IoT Hub is received.
 * The received data will then be displayed in the standard output.
 */
static IOTHUBMESSAGE_DISPOSITION_RESULT ReceiveMessageCallback(
        IOTHUB_MESSAGE_HANDLE message, void* userContextCallback) {
    int* counter = (int*) userContextCallback;
    const unsigned char* buffer = NULL;
    size_t size = 0;
    const char* messageId;
    const char* correlationId;

    // AMQP message properties
    if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL) {
        messageId = "<null>";
    }

    if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL) {
        correlationId = "<null>";
    }

    // AMQP message content.
    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(
            message);

    if (contentType == IOTHUBMESSAGE_BYTEARRAY) {
        if (IoTHubMessage_GetByteArray(message, &buffer, &size)
                == IOTHUB_MESSAGE_OK) {
            printf("Received Message [%d] (message-id: %s, correlation-id: %s)"
                    " with BINARY Data: <<<%.*s>>> & Size=%d\r\n", *counter,
                    messageId, correlationId, (int) size, buffer, (int) size);
        } else {
            printf("Failed getting the BINARY body of the "
                    "message received.\r\n");
        }
    } else if (contentType == IOTHUBMESSAGE_STRING) {
        if ((buffer = (const unsigned char*) IoTHubMessage_GetString(message))
                != NULL && (size = strlen((const char*) buffer)) > 0) {
            printf("Received Message [%d] (message-id: %s, correlation-id: %s)"
                    " with STRING Data: <<<%.*s>>> & Size=%d\r\n", *counter,
                    messageId, correlationId, (int) size, buffer, (int) size);

            // If we receive the work 'quit' then we stop running
        } else {
            printf("Failed getting the STRING body of the "
                    "message received.\r\n");
        }
    } else {
        printf("Failed getting the body of the message received (type %i).\r\n",
                contentType);
    }

    // Retrieve properties from the message
    MAP_HANDLE mapProperties = IoTHubMessage_Properties(message);
    if (mapProperties != NULL) {
        const char* const * keys;
        const char* const * values;
        size_t propertyCount = 0;
        if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount)
                == MAP_OK) {
            if (propertyCount > 0) {
                size_t index;

                printf("Message Properties:\r\n");
                for (index = 0; index < propertyCount; index++) {
                    printf("\tKey: %s Value: %s\r\n", keys[index],
                            values[index]);
                }
                printf("\r\n");
            }
        }
    }

    if (memcmp(buffer, "quit", size) == 0) {
        g_continueRunning = false;
    }

    /* Some device specific action code goes here... */
    (*counter)++;
    return IOTHUBMESSAGE_ACCEPTED;
}

/**
 * Callback function that will be called when a send message is confirmed
 * by Microsoft* Azure* IoT Hub.
 * The result of the sending operation will be displayed in the standard output.
 */
static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result,
        void* userContextCallback) {
    EVENT_INSTANCE* eventInstance = (EVENT_INSTANCE*) userContextCallback;
    printf("Confirmation[%d] received for message tracking "
            "id = %zu with result = %s\n", callbackCounter,
            eventInstance->messageTrackingId,
            ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));

    /* Some device specific action code goes here... */
    callbackCounter++;
    IoTHubMessage_Destroy(eventInstance->messageHandle);
}

// check if running as root
void checkRoot(void)
{
    int euid = geteuid();
    if (euid) {
        cerr << "This project uses Mraa I/O operations that require\n"
            "'root' privileges, but you are running as non - root user.\n"
            "Passwordless keys(RSA key pairs) are recommended \n"
            "to securely connect to your target with root privileges. \n"
            "See the project's Readme for more info.\n\n";
    }
    return;
}

void initPlatform(int& aPin)
{
    // check which board we are running on
    Platform platform = getPlatformType();
    switch (platform) {
    case INTEL_UP2:
        aPin = 2;  	// A2 Connector
#ifdef USING_GROVE_PI_SHIELD
        aPin = 2 + 512; // A2 connector, 512 offset needed for the shield
        break;
#endif
    default:
        string unknownPlatformMessage = "This sample uses the MRAA/UPM library for I/O access, "
            "you are running it on an unrecognized platform. "
            "You may need to modify the MRAA/UPM initialization code to "
            "ensure it works properly on your platform.\n\n";
        cerr << unknownPlatformMessage;
    }
    return;
}

int main(void) {

    // check if running as root
    checkRoot();

#ifndef SIMULATE_DEVICES
    int aPin;
    initPlatform(aPin);
    
#ifdef USING_GROVE_PI_SHIELD
    addSubplatform(GROVEPI, "0");
#endif


    // Initialize temperature sensor connected to an analog in pin
    upm::GroveTemp* temp_sensor = new upm::GroveTemp(aPin);
    if (temp_sensor == NULL) {
        fprintf(stderr, "ERROR: MRAA couldn't initialize Analog interface, "
                "exiting");
        return mraa::ERROR_UNSPECIFIED;
    }
#endif
    // Initialize the Azure IoT SDK
    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
    EVENT_INSTANCE messages[MESSAGE_COUNT];
    int receiveContext = 0;
    g_continueRunning = true;
    callbackCounter = 0;

#ifndef SIMULATE_DEVICES
    if (platform_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize the platform.\n");
        return mraa::ERROR_UNSPECIFIED;
    }
#endif
    printf("Starting the IoTHub client sample AMQP...\n");

    if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(
            connectionString, AMQP_Protocol)) == NULL) {
        fprintf(stderr, "ERROR: iotHubClientHandle is NULL!\n");
        return mraa::ERROR_UNSPECIFIED;
    }

    bool traceOn = true;
    IoTHubClient_LL_SetOption(iotHubClientHandle, "logtrace", &traceOn);
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts",
            certificates) != IOTHUB_CLIENT_OK) {
        fprintf(stderr, "ERROR: failure to set option \"TrustedCerts\"\r\n");
        return mraa::ERROR_UNSPECIFIED;
    }

    /* Setting Message call back, so we can receive Commands. */
    if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle,
            ReceiveMessageCallback, &receiveContext) != IOTHUB_CLIENT_OK) {
        fprintf(stderr, "ERROR: IoTHubClient_LL_SetMessageCallback.........."
                "FAILED!\n");
        return mraa::ERROR_UNSPECIFIED;
    }
    printf("IoTHubClient_LL_SetMessageCallback...successful.\n");

    /* Now that we are ready to receive commands, let's send some messages */
    size_t iterator = 0;
    do {
        if (iterator < MESSAGE_COUNT) {
            float analogValue = 0.0f;
#ifndef SIMULATE_DEVICES
            analogValue = temp_sensor->raw_value();
#else
            analogValue += 1.0f;
#endif
            sprintf_s(msgText, sizeof(msgText),
                    "{\"deviceId\": \"myFirstDevice\",\"temperature\": %.2f}",
                    analogValue);
            if ((messages[iterator].messageHandle =
                    IoTHubMessage_CreateFromByteArray(
                            (const unsigned char*) msgText, strlen(msgText)))
                    == NULL) {
                fprintf(stderr, "ERROR: iotHubMessageHandle is NULL!\n");
            } else {
                messages[iterator].messageTrackingId = iterator;

                MAP_HANDLE propMap = IoTHubMessage_Properties(
                        messages[iterator].messageHandle);
                sprintf_s(propText, sizeof(propText), "PropMsg_%zu", iterator);
                if (Map_AddOrUpdate(propMap, "PropName", propText) != MAP_OK) {
                    fprintf(stderr, "WARN: Map_AddOrUpdate Failed!\n");
                }

                if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle,
                        messages[iterator].messageHandle,
                        SendConfirmationCallback, &messages[iterator])
                        != IOTHUB_CLIENT_OK) {
                    fprintf(stderr, "ERROR: IoTHubClient_LL_SendEventAsync "
                            "failed to send message [%zu]! to IoT Hub\n",
                            iterator);
                } else {
                    printf("IoTHubClient_LL_SendEventAsync accepted message "
                            "[%zu] for transmission to IoT Hub.\n", iterator);
                }
            }
        }
        IoTHubClient_LL_DoWork(iotHubClientHandle);
        ThreadAPI_Sleep(1);

        iterator++;
    } while (g_continueRunning);

    printf("iothub_client_sample_amqp has gotten quit message, call DoWork"
            " %d more time to complete final sending...\n", DOWORK_LOOP_NUM);
    for (size_t index = 0; index < DOWORK_LOOP_NUM; index++) {
        IoTHubClient_LL_DoWork(iotHubClientHandle);
        ThreadAPI_Sleep(1);
    }
    IoTHubClient_LL_Destroy(iotHubClientHandle);
    platform_deinit();
}
