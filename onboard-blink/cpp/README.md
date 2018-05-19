# Onboard Blink 


## Introduction
This is a simple sample you could use for a quick test.

## What it is
A blink sample, it turns the on-board LED on for one second, then off for one second, repeatedly.

## Hardware requirements

A board with an accesible GPIO pin (connected to an LED).

## Supported boards

This sample has been tested on
- MinnowBoard Turbot 2.0
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements
This version of the sample has been tested on Ubuntu Linux. It requires the [mraa library](https://github.com/intel-iot-devkit/mraa). 


## Setup
Create a new project on Intel(R) System Studio using this sample. Make sure the on board LED or the external LED is connected to the board.

## Note

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
