# Microsoft Azure HTTP Sample


## Introduction
This sample demonstrates using Microsoft Azure IoT Hub to stream information gathered by a connected sensor to a cloud service.


## Hardware requirements

[Grove Temperature Sensor](http://wiki.seeed.cc/Grove-Temperature_Sensor/)*

Target device should be connected to the internet.

\* See the note below if you don't have access to a board or this sensor.

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries.

libcurl: On your target Ubuntu system do: `apt install libcurl3`


## Setup

* Sign in to your Microsoft Azure account or open a new account if you don't already have one. You will need a personal email address, and a credit card (to verify identity).
* In Azure dashboard search for IoT, select IoT Hub, create a new hub, name it, select FREE Tier, fill in other info, continue to create a Hub.
    Wait for the hub to be deployed

* Use one of the methods listed in ‘Create new device in the IoT Hub device identity registry’ in [this page](https://github.com/Azure/azure-iot-device-ecosystem/blob/master/setup_iothub.md#manage-an-azure-iot-hub)

    For example to use the cli on mac, first install Azure cli, see [this page](https://docs.microsoft.com/en-us/cli/azure/install-azure-cli-macos?view=azure-cli-latest)

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

* Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
