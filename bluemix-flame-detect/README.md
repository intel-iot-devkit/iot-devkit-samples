# IBM® Cloud (foremerly Bluemix) flame detect Sample


## Introduction
This sample demonstrates using IBM® Bluemix™ to stream information gathered by a connected sensor to a cloud service.


## Hardware requirements

[Grove* Buzzer](http://wiki.seeed.cc/Grove-Buzzer/)*, [Grove Flame Sensor](http://wiki.seeed.cc/Grove-Flame_Sensor/)*
Target device should be connected to the internet.

* See the note below if you don't have access to these devices.

## Supported boards

This sample has been tested on
- Up Squared* board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries
[paho mqtt C++ Client](https://www.eclipse.org/paho/clients/cpp/)

You can build the paho mqtt library from source, or use [this link](https://www.eclipse.org/downloads/download.php?file=/paho/1.3/eclipse-paho-mqtt-c-unix-1.2.0.tar.gz) to download a pre-built library. After expanding the zipped file, copy the 'libpaho-mqtt3c.so.1' into 
/usr/lib folder on your target system.

## Setup

Refer to [this page](https://console.bluemix.net/) to get started on IBM®  Bluemix™. Also see [this guide](https://github.com/intel-iot-devkit/iot-samples-cloud-setup/blob/master/bluemix-mqtt.md) for further help on initial setup.

Create a new project on Intel(R) System Studio. After choosing the Bluemix Quickstart sample, it'll ask you for a 'Device ID'. Enter a string of your choosing as ID to identify messages from your device in Bluemix console.

## Notes

If you don't have a temperature sensor or LED, you can still run the sample to see how it sends info to
Bluemix clould service. In main.cpp, remove the comment to define SIMULATE_DEVICES.

Accessing device sensors, including LEDs, requires MRAA I/O operations. To avoid permission issues:

1. If you've already connected to your target, you'll need to kill the target tcf-agent and reboot the board before connecting with elevated privileges. More information is available [here](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-creating-an-ssh-connection).

2. Connect to the target using passwordless (recommended) or with password-based SSH.

Passwordless Public Key-based Method (recommended):
First, generate the public keys. For instructions, see [Set up a new connection for your target](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-2019-beta-creating-an-ssh-connection). 
Second, use the generated keys to connect to the target. For instructions, see [Login using public key](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-2019-beta-connecting-to-target).

Password-based Method: 
Log in as the root user to connect to the target. For instructions, see [Authentication using password](https://software.intel.com/en-us/developing-projects-with-intel-system-studio-c-2019-beta-connecting-to-target). Because of security concerns, this method is not recommended.


## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.
