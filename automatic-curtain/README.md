# Automatic Curtain


## Introduction
This sample opens and closes a curtain automatically according to the luminosity set by a Grove* Rotary Angle Sensor. 


## Hardware requirements

[Grove Rotary Angle Sensor](http://wiki.seeed.cc/Grove-Rotary_Angle_Sensor/) connected to an analog port.
[Grove Light Sensor](http://wiki.seeed.cc/Grove-Light_Sensor/) connected to an analog port.
[Grove Button](http://wiki.seeed.cc/Grove-Button/) connected to a digital port.
[Jhd1313m1 LCD](https://iotdk.intel.com/docs/master/upm/classupm_1_1_jhd1313m1.html) connected to any I2C.
[Stepper Motor Driver Uln200xa](https://iotdk.intel.com/docs/master/upm/classupm_1_1_u_l_n200_x_a.html) connected in this way:
	 -- I1  ->  pin D8\n
     -- I2  ->  pin D9\n
     -- I3  ->  pin D10\n
     -- I4  ->  pin D11\n
     -- GND ->  GND\n
     -- Vcc ->  5V (Vcc)\n
     -- Vm  ->  NC (Not Connected)
Connect directly to the Up Squared* board pin or to the extended heads on the GrovePi+ shield for this.
A 5V power supply is required in order to run this example.

* See the note below if you don't have access to these devices.

This sample assumes that the :
  - curtain is initially open;
  - to close completely the courtain 2 stepper motor's full revolutions are
    needed;
  - lux range in [0-60].

## Supported boards

This sample has been tested on
- Up Squared board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

## Setup
Follow the steps listed in the hardware requirements section.

## Notes

If you don't have the sensors needed, you can still run the sample by uncommenting the definition of SIMULATE_DEVICES.

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