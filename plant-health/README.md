# Plant Health Sample

## Introduction
This example helps simulates a plant health monitoring system.


## Hardware requirements

[Grove Moisture Sensor]((https://www.seeedstudio.com/s/moisture.html)  connected to an analog in port.
[Grove Temperature Sensor](http://wiki.seeed.cc/Grove-Temperature_Sensor/)* connected to an analog port
[Grove UV Sensor](https://www.seeedstudio.com/Grove-UV-Sensor-p-1540.html) connected to an analog in port.
[Grove UV Sensor](http://wiki.seeed.cc/Grove-Dry-Reed_Relay/) connected to an analog in port.
[Jhd1313m1 LCD](https://iotdk.intel.com/docs/master/upm/classupm_1_1_jhd1313m1.html) connected to any I2C

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

## Setup
Create a new project on Intel(R) System Studio using this sample. 
Connect the input device to an analog input pin on your IoT board

## Notes

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).

## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.

