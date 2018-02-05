# IBM® Cloud (foremerly Bluemix) QuickStart Sample


## Introduction
This sample demonstrates using IBM® Bluemix™ to stream information gathered by a connected sensor to a cloud service.


## Hardware requirements

[Grove LED](http://wiki.seeed.cc/Grove-LED_Socket_Kit/)*, [Grove Temperature Sensor](http://wiki.seeed.cc/Grove-Temperature_Sensor/)*
Target device should be connected to the internet.

* See the note below if you don't have access to these devices.

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries
[paho mqtt C++ Client](https://www.eclipse.org/paho/clients/cpp/)

You can build the paho mqtt library from source, or use [this link](https://www.eclipse.org/downloads/download.php?file=/paho/1.3/eclipse-paho-mqtt-c-unix-1.2.0.tar.gz) to download a pre-built library. After expanding the zipped file, copy the 'libpaho-mqtt3c.so.1' into 
/usr/lib folder on your target system.

## Setup

Refer to [this page](https://console.bluemix.net/) to get started on IBM®  Bluemix™. Also see [this guide](https://github.com/intel-iot-devkit/iot-samples-cloud-setup/blob/master/bluemix-mqtt.md) for further help on initial setup.


Create a new project on Intel(R) System Studio. After choosing the Bluemix Quickstart sample, it'll ask you for information such as 'Device ID', 'Device Type', etc. Enter the information and your credentials that you created while setting up your account on Bluemix console.

## Notes

If you don't have the sensor and buzzer, you can still run the sample to see how it sends info to
Bluemix clould service. In main.cpp, remove the comment to define SIMULATE_DEVICES.

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
