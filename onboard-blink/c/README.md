# Onboard Blink 


## Introduction
Turns the on-board LED on for one second, then off for one second, repeatedly.

## What it is
A blink sample

## Hardware requirements

No special H/W requirements.
The sample needs minor modification depending on the board and shield you are using, if any.

## Preferred boards

  MinnowBoard Turbot 2.0
  Up2 board

## Software requirements
This version of the sample has been tested on Ubuntu Linux. It uses the mraa library. 

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available here: https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md

## Setup
Create a new project on Intel(R) System Studio using this sample. Make sure the on board LED or the external LED is connected to the board.
