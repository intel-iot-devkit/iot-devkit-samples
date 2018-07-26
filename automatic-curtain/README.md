# Automatic Curtain


## Introduction
This sample opens and closes a curtain automatically according to the luminosity set by a Grove Rotary Angle Sensor. 


## Hardware requirements

[Grove Rotary Angle Sensor](http://wiki.seeed.cc/Grove-Rotary_Angle_Sensor/)connected to an analog port
[Grove Light Sensor](http://wiki.seeed.cc/Grove-Light_Sensor/) connected to an analog port
[Grove Button](http://wiki.seeed.cc/Grove-Button/) connected to a digital port
[Jhd1313m1 LCD](https://iotdk.intel.com/docs/master/upm/classupm_1_1_jhd1313m1.html) connected to any I2C
[Stepper Motor Driver Uln200xa](https://iotdk.intel.com/docs/master/upm/classupm_1_1_u_l_n200_x_a.html) connected in this way:
	 -- I1  ->  pin D8\n
     -- I2  ->  pin D9\n
     -- I3  ->  pin D10\n
     -- I4  ->  pin D11\n
     -- GND ->  GND\n
     -- Vcc ->  5V (Vcc)\n
     -- Vm  ->  NC (Not Connected)
Connect directly to the Up2 board pin or to the extended heads on the Grove Pi+ shield for this.
A 5V power supply is required in order to run this example

* See the note below if you don't have access to these devices.

This sample assumes that the :
  - curtain is initially open;
  - to close completely the courtain 2 stepper motor's full revolutions are
    needed;
  - lux range in [0-60].

## Supported boards

This sample has been tested on
- Up2 board

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

## Setup
Follow the steps listed in the hardware requirements section.

## Notes

If you don't have the sensors needed, you can still run the sample by uncommenting the definition of SIMULATE_DEVICES.

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).

## Disclaimer
IMPORTANT NOTICE: This software is sample software. It is not designed or intended for use in any medical, life-saving or life-sustaining systems, transportation systems, nuclear systems, or for any other mission-critical application in which the failure of the system could lead to critical injury or death. The software may not be fully tested and may contain bugs or errors; it may not be intended or suitable for commercial release. No regulatory approvals for the software have been obtained, and therefore software may not be certified for use in certain countries or environments.