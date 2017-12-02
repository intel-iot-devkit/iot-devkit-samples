# Tan Responsibly


## Introduction
This example helps you to avoid sunburn by using a UV and temperature sensor to find the UV intensity and alert a buzzer when sunburn conditions occurs.


## Hardware requirements

[Grove UV Sensor](https://www.seeedstudio.com/Grove-UV-Sensor-p-1540.html) connected to an analog in port.
[Grove Temperature Sensor](http://wiki.seeed.cc/Grove-Temperature_Sensor_V1.2/)connected to an analog in port.
[Grove Buzzer](http://wiki.seeed.cc/Grove-Buzzer/) connected to a digital out port.
[Jhd1313m1 LCD](https://iotdk.intel.com/docs/master/upm/classupm_1_1_jhd1313m1.html) connected to any I2C

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

## Setup
Follow the steps listed in the hardware requirements section.

## Notes

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
