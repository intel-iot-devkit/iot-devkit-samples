# PWM


## Introduction
This is a simple sample you could use for a quick test of PWM output.

## What it is

This project demonstrates how to write a value to an output pin using the MRAA library.
If the output is connected to a led, its intensity, as perceived by the human eye, will vary depending on the duty cycle.


## Hardware requirements

A board with an accesible GPIO input pin.
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
Connect the input device to a PWM pin on your IoT board

## Note

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
