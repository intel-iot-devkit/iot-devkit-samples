# Digital In


## Introduction
This is a simple sample you could use for a quick test of digital input.

## What it is

This project demonstrates how to read a digital value from an input pin using the MRAA library.

## Hardware requirements

A board with an accesible GPIO input pin.
Some input device or sensor such as the 'Button' or 'Touch Sensor' in 'Seeed Grove Starter Kit'.

## Supported boards

This sample has been tested on
- MinnowBoard Turbot 2.0
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements
This version of the sample has been tested on Ubuntu Linux. It requires the [mraa library](https://github.com/intel-iot-devkit/mraa). 

## Setup
Create a new project on Intel(R) System Studio using this sample. 
Connect the input device to a digital input pin on your IoT board

## Note

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
