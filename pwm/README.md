# PWM


## Introduction
This is a simple sample you could use for a quick test of PWM output.

## What it is

This project demonstrates how to write a value to an output pin using the MRAA library.
If the output is connected to a led, its intensity, as perceived by the human eye, will vary depending on the duty cycle.


## Hardware requirements

A board with an accesible PWM pin. 
Some output device such as an LED 'Seeed Grove Starter Kit', or just use a multimeter connected to the pin.

## Supported boards

This sample has been tested on
- MinnowBoard Turbot 2.0
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements
This version of the sample has been tested on Ubuntu Linux. It requires the [mraa library](https://github.com/intel-iot-devkit/mraa). 

## Setup
Create a new project on Intel(R) System Studio using this sample. 
Connect the input device to a PWM pin on your IoT board. To find a valid PWM pin refer to [Pin map](https://32414320wji53mwwch1u68ce-wpengine.netdna-ssl.com/wp-content/uploads/2013/07/grovepi_pinout.png)

## Note

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).

## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.

