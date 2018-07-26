# ISR

## Introduction
This is a simple sample you could use for a quick test of interrupt service routine.

## What it is

Demonstrate how to react on an external event with an ISR (Interrupt Service Routine), which will run independently of the main program flow using the MRAA library.

## Hardware requirements

Use a platform with GPIO Interrupt capabilities
Any button or sensor that can generate a voltage transition from ground to Vcc or vice versa can be used with this example code.
Suitable ones in the Grove Starter Kit are the Button and Touch Sensor, connected to digital a pin.

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements
This version of the sample has been tested on Ubuntu Linux. It requires the [mraa library](https://github.com/intel-iot-devkit/mraa). 

## Setup
Create a new project on Intel(R) System Studio using this sample. 
Connect the input device to a PWM pin on your IoT board. To find a valid PWM pin refer to [Pin map](https://32414320wji53mwwch1u68ce-wpengine.netdna-ssl.com/wp-content/uploads/2013/07/grovepi_pinout.png)

## Note

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
