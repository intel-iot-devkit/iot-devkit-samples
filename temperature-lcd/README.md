# Display Temperature on LCD Sample

## Introduction
This sample demonstrates use of a temperature sensor and LCD in SEED Grove Starter Kit.

## What it is
This project template includes the starting code needed to use the various sensors that come with the Grove Starter Kit.
Update the temperature values and reflect the changes on the LCD.

## Hardware requirements

Grove LED (GroveLed)
Grove Button (GroveButton)
Grove Temperature Sensor (GroveTemp)
Grove LCD (Jhd1313m1)

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

This version of the sample has been tested on Ubuntu Linux.

The following libraries need to be installed on your target platfrom for this sample to work:
[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

## Setup

Create a new project on Intel(R) System Studio using this sample. 
For connections refer to [Pin map](https://32414320wji53mwwch1u68ce-wpengine.netdna-ssl.com/wp-content/uploads/2013/07/grovepi_pinout.png)

## Notes

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).

## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.
