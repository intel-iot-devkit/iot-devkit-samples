# UP Squared* Built-in LEDs

## Introduction
This is a simple sample that can be used to blink the built-in LEDs on the UP Squared board.

## What it is

The main purpose of this sample is to showcase the new LED class and APIs added to the MRAA library.

## Hardware requirements

An [UP Squared](http://www.up-board.org/) board. No additional hardware required.

## Supported boards

This sample is intended for the [UP Squared](http://www.up-board.org/) board.

However, with minor modifications it will run on any Linux board that exposes built-in LEDs using the
gpio-leds Linux kernel driver.

## Software requirements
This version of the sample has been tested on Ubuntu Linux but should be compatible with Ubilinux for the UP Squared as well.
It requires the [mraa library](https://github.com/intel-iot-devkit/mraa) version 1.9.0 or newer. 

## Setup
Create a new project on Intel(R) System Studio using this sample. Run it on the UP Squared board using a TCF connection.

## Note

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
