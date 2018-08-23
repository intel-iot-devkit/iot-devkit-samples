# Microsoft Azure* HTTP Sample


## Introduction
This sample demonstrates using Microsoft Azure IoT Hub to stream information gathered by a connected sensor to a cloud service.


## Hardware requirements

[Grove* Temperature Sensor](http://wiki.seeed.cc/Grove-Temperature_Sensor/)*

Target device should be connected to the internet.

\* See the note below if you don't have access to a board or this sensor.

## Supported boards

This sample has been tested on
- Up Squared* board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

libcurl: On your target Ubuntu system do: `apt install libcurl3`


## Setup

* Sign in to your Microsoft Azure account or open a new account if you don't already have one. You will need a personal email address, and a credit card (to verify identity).
* In Azure dashboard search for IoT, select IoT Hub, create a new hub, name it, select FREE Tier, fill in other info, continue to create a Hub.
    Wait for the hub to be deployed

* Use one of the methods listed in ‘Create new device in the IoT Hub device identity registry’ in [this page](https://github.com/Azure/azure-iot-device-ecosystem/blob/master/setup_iothub.md#manage-an-azure-iot-hub).

    For example to use the cli on mac, first install Azure cli, see [this page](https://docs.microsoft.com/en-us/cli/azure/install-azure-cli-macos?view=azure-cli-latest).

* Add a device to your hub, e.g.
`az iot device create --hub-name MyIotHub --device-id MyDevice`

* Go to Azure dashboard > Iot Hub > IoT Devices
    Select the device you just created, copy the ‘Connection String’, you will need it in the next step.

* Open Intel(R) System Studio. Start the 'New Project Creation Wizard' (File > New > Example project for building on a container and running on Linux). Choose Ubuntu, C++, Cloud, and select 'Azure IoT Hub with HTTP' sample.  It'll ask you for 'connection string'. Copy the string you got from the Azure Dashbord in the previous step and continue to create a new project based on this sample.

* After creating the new project, right click the project and go to properties. Go to `C/C++ General` > `Paths and Symbols` view and open the `GNU C++` Language. 

	Delete the following entries: 

	* `//${DOCKER_IMAGE}${DOCKER_SYSROOT}/usr/include/azureiot`
	* `//${DOCKER_IMAGE}${DOCKER_SYSROOT}/usr/include/azureiot/inc`

	Add the following entries:

	* `//includes-${DOCKER_IMAGE}.${DOCKER_IMAGE_TAG}/usr/include/azureiot`
	* `//includes-${DOCKER_IMAGE}.${DOCKER_IMAGE_TAG}/usr/include/azureiot/inc`

	Save your changes. Then, right-click the project again and select `Index` > `Rebuild` to rebuild your includes list.

## Notes

* If you don't have an IoT board or the sensor, you can still run the sample to see how it sends info to
Azure IoT Hub. In `main.cpp` remove the comment to define `SIMULATE_DEVICES`.

* Accessing device sensors, including LEDs, requires MRAA I/O operations. To avoid permission issues:

1. If you've already connected to your target, you'll need to kill the target tcf-agent and reboot the board before connecting with elevated privileges. More information is available [here](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-creating-an-ssh-connection).

2. Connect to the target using passwordless (recommended) or with password-based SSH.

Passwordless Public Key-based Method (recommended):
First, generate the public keys. For instructions, see [Set up a new connection for your target](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-2019-beta-creating-an-ssh-connection). 
Second, use the generated keys to connect to the target. For instructions, see [Login using public key](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-2019-beta-connecting-to-target).

Password-based Method: 
Log in as the root user to connect to the target. For instructions, see [Authentication using password](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-2019-beta-connecting-to-target). Because of security concerns, this method is not recommended.


## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.
