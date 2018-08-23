# ISR

## Introduction
This is a simple sample you could use for a quick test of interrupt service routine.

## What it is

Demonstrate how to react on an external event with an ISR (Interrupt Service Routine), which will run independently of the main program flow using the MRAA library.

## Hardware requirements

Use a platform with GPIO Interrupt capabilities.
Any button or sensor that can generate a voltage transition from ground to Vcc or vice versa can be used with this example code.
Suitable ones in the Grove* Starter Kit are the Button and Touch Sensor, connected to digital a pin.

## Supported boards

This sample has been tested on
- Up Squared* board

The sample might need minor modifications depending on the board and shield you are using.
Note: This sample does not work for the GPIO pins on the GrovePi+* shield.

## Software requirements
This version of the sample has been tested on Ubuntu Linux. It requires the [mraa library](https://github.com/intel-iot-devkit/mraa). 

## Setup
Create a new project on Intel(R) System Studio using this sample.
Connect the input device to a GPIO pin on your IoT board (pin 13 is used by default on most boards).

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
