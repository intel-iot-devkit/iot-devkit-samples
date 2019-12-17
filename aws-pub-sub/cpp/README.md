# AWS IoT PubSub Sample


## Introduction
This app is a version of Amazon Web Services (AWS) IoT C++ PubSub Sample, modified for ease of creating a new Intel(R) System Studio project.

## What it is
PubSub sample demonstrates data exchanges of a connected device (Thing) with AWS IoT cloud using the AWS IoT C++ SDK.
For more information see the [AWS IoT SDK Page](https://github.com/aws/aws-iot-device-sdk-cpp).

## Hardware requirements

No special H/W requirements, as long as your device could be connected to the internet.

## Software requirements

This version of the sample has been tested on Ubuntu Linux.

This sample requires additional system configuration when using Ubuntu OS with the UP series boards. Instructions on how to install the custom provided Linux kernel with the required drivers can be [found here](https://wiki.up-community.org/Ubuntu#Ubuntu_18.04_installation_and_configuration).

## Setup

Refer to [this document](https://github.com/intel-iot-devkit/iot-samples-cloud-setup/blob/master/aws-mqtt.md) on how to create an AWS account and setup the AWS IoT. When the initial account setup is done, make a note of the endpoint corresponding to your account, also download the certificate files into a known folder on your target system (e.g. /etc/aws-certs).

Create a new project on Intel(R) System Studio. After choosing the PubSub sample, it'll ask you for the endpoint and the certificate filenames. 

## Note
In the original PubSub sample, the configuration info is read from the SampleConfig.json file. But in this version, during the new project creation a new file, 'src/credentials.h' is generated that keeps the configuration information you entered in the wizard. Afterwards, you can modify this header file if the information changes. Also if you need to modify other configuration information, see the defines in 'src/common/ConfigCommon.cpp'.

## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.
