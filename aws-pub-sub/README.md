# AWS® IOT Pub-Sub Sample


## Introduction

This sample is a version of Amazon Web Services (AWS) IoT C++ PubSub Sample, modified for ease of creating a new Intel(R) System Studio project. It demonstrates data exchanges of a connected device (Thing) with AWS IoT cloud using the AWS IOT C++ SDK.


## Hardware requirements

This sample does not need any special hardware. The payload message can be replaced by any sensor data. The target device needs to be connected to the internet.

## Supported boards

This sample has been tested on
- Up2 board


## Software requirements

None


## Setup


Refer to [this document](https://github.com/intel-iot-devkit/iot-samples-cloud-setup/blob/master/aws-mqtt.md) on how to create an AWS account and setup the AWS IoT. When the initial account setup is done, make a note of the endpoint corresponding to your account, also download the certificate files into a known folder on your target system (e.g. /etc/aws-certs).

Create a new project on Intel(R) System Studio. After choosing the PubSub sample, it'll ask you for the endpoint and the certificate filenames. 

## Notes
In the original PubSub sample, the configuration info is read from the SampleConfig.json file. But in this version, during the new project creation a new file, 'src/credentials.h' is generated that keeps the configuration information you entered in the wizard. Afterwards, you can modify this header file if the information changes. Also if you need to modify other configuration information, see the defines in 'src/common/ConfigCommon.cpp'.