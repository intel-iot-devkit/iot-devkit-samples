/*
 * Microsoft Azure IoT SDKs
 * Copyright (c) Microsoft Corporation
 * All rights reserved.
 *
 * Author: Thomas Lyet <thomas.lyet@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

/* This sample uses the _LL APIs of iothub_client for example purposes.
 That does not mean that HTTP only works with the _LL APIs.
 Simply changing using the convenience layer (functions not having _LL)
 and removing calls to _DoWork will yield the same results. */

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_client_ll.h"
#include "iothub_message.h"
#include "iothubtransportmqtt.h"

#include <iostream>

#include "credentials.h"
#include "certs.h"
#include <mraa.hpp>
#include <grove.hpp>

/**
 * This project template allow you to test the Microsoft* Azure* IoT Hub.
 * It will read the temperature from the Grove Temperature Sensor and send
 * it to the Azure* IoT Hub.
 *
 * First steps:
 * - Create an IoT HuB following the instruction in
 * https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
 * - Create a connection string for a device following the instruction in
 * https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
 * - Copy the connection string inside the "credentials.h" header
 * - Run the example and check the results inside your Azure* IoT Hub
 *
 * * Other names and brands may be claimed as the property of others.
 */

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_CONFIRMATION_RESULT,
        IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES);

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
    if (userContextCallback != NULL) {
        int* counter = (int*) userContextCallback;
        const char* buffer;
        size_t size;
        MAP_HANDLE mapProperties;

        if (IoTHubMessage_GetByteArray(message, (const unsigned char**) &buffer,
                &size) != IOTHUB_MESSAGE_OK) {
            fprintf(stderr, "ERROR: Unable to retrieve the message data\n");
        } else {
            printf("Received Message [%d] with Data: <<<%.*s>>> & Size=%d\n",
                    *counter, (int) size, buffer, (int) size);
            if (memcmp(buffer, "quit", size) == 0) {
                g_continueRunning = false;
            }
        }

        // Retrieve properties from the message
        mapProperties = IoTHubMessage_Properties(message);
        if (mapProperties != NULL) {
            const char* const * keys;
            const char* const * values;
            size_t propertyCount = 0;
            if (Map_GetInternals(mapProperties, &keys, &values, &propertyCount)
                    == MAP_OK) {
                if (propertyCount > 0) {
                    size_t index;

                    printf("Message Properties:\n");
                    for (index = 0; index < propertyCount; index++) {
                        printf("\tKey: %s Value: %s\n", keys[index],
                                values[index]);
                    }
                    printf("\n");
                }
            }
        }

        /* Some device specific action code goes here... */
        (*counter)++;
    }
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

int main(void) {
    // Initialize temperature sensor connected to A0 (analog in)
    upm::GroveTemp* temp_sensor = new upm::GroveTemp(0);
    if (temp_sensor == NULL) {
        fprintf(stderr, "ERROR: MRAA couldn't initialize Analog interface, "
                "exiting");
        return mraa::ERROR_UNSPECIFIED;
    }

    // Initialize the Azure IoT SDK
    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
    EVENT_INSTANCE messages[MESSAGE_COUNT];
    int receiveContext = 0;
    g_continueRunning = true;
    callbackCounter = 0;

    if (platform_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize the platform.\n");
        return mraa::ERROR_UNSPECIFIED;
    }
    printf("Starting the IoTHub client sample MQTT...\n");

    if ((iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(
            connectionString, MQTT_Protocol)) == NULL) {
        fprintf(stderr, "ERROR: iotHubClientHandle is NULL!\n");
        return mraa::ERROR_UNSPECIFIED;
    }
    bool traceOn = true;
    IoTHubClient_LL_SetOption(iotHubClientHandle, "logtrace", &traceOn);
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "TrustedCerts",
            certificates) != IOTHUB_CLIENT_OK) {
        printf("failure to set option \"TrustedCerts\"\r\n");
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
            float analogValue = temp_sensor->raw_value();
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

    printf("iothub_client_sample_http has gotten quit message, call DoWork"
            " %d more time to complete final sending...\n",
    DOWORK_LOOP_NUM);
    for (size_t index = 0; index < DOWORK_LOOP_NUM; index++) {
        IoTHubClient_LL_DoWork(iotHubClientHandle);
        ThreadAPI_Sleep(1);
    }
    IoTHubClient_LL_Destroy(iotHubClientHandle);
    platform_deinit();
}
