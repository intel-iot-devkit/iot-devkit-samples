# Bluemix flame detect Sample


## Introduction
This sample demonstrates using IBM® Bluemix™ to store information gathered be a sensor in a 
clould service.


## Hardware requirements

[Grove Buzzer](http://wiki.seeed.cc/Grove-Buzzer/)*, [Grove Flame Sensor](http://wiki.seeed.cc/Grove-Flame_Sensor/)*
Target device should be connected to the internet.

* See the note below if you don't have access to these devices.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries
[paho mqtt C++ Client](https://www.eclipse.org/paho/clients/cpp/)

You can build the paho mqtt library from source, or use [this link](https://www.eclipse.org/downloads/download.php?file=/paho/1.3/eclipse-paho-mqtt-c-unix-1.2.0.tar.gz) to download a pre-built library. After expanding the zipped file, copy the 'libpaho-mqtt3c.so.1' into 
/usr/lib folder on your target system.

## Setup

Refer to [this page](https://console.bluemix.net/) to get started on IBM®  Bluemix™.

Create a new project on Intel(R) System Studio. After choosing the Bluemix Quickstart sample, it'll ask you for a 'Device ID'. Enter something so you can identify messages from your device in Bluemix console.

## Note

If you don't have a temperature sensor or LED, you can still run the sample to see how it sends info to
Bluemix clould service. In main.cpp, remove the comment to define SIMULATE_DEVICES.
