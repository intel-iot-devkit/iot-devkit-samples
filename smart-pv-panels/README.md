# Smart Photo Voltaic Panels

## Introduction
This example helps you to move photovoltaic panel towards the maximum brightness of the sun. The idea is to move photovoltaic panel (represented by a motor) according to the movement of the sun, directing it towards the maximum light radiated during the day using the maximum light intensity detected by two light sensor placed parallel to each other.

It computes the sensors average light in a calibration step and use this number to move motor according to the movement of light.


## Hardware requirements

2 [Grove Light Sensor](http://wiki.seeed.cc/Grove-Light_Sensor/) connected to different ports.
[Jhd1313m1 LCD](https://iotdk.intel.com/docs/master/upm/classupm_1_1_jhd1313m1.html) connected to any I2C
[Stepper Motor Driver Uln200xa](https://iotdk.intel.com/docs/master/upm/classupm_1_1_u_l_n200_x_a.html)
 	 -- I1  ->  pin D6\n
 	 -- I2  ->  pin D7\n
     -- I3  ->  pin D8\n
     -- I4  ->  pin D9\n
     -- GND ->  GND\n
     -- Vcc ->  5V (Vcc)\n
    -- Vm  ->  NC (Not Connected)

Connect the stepper motor and stepper motor driver directly to the Up2 board pin set or to the extended heads on the Grove Pi+ shield.

## Supported boards

The sample might need minor modifications depending on the board and shield you are using.

## Software requirements

The following libraries need to be installed on your target platfrom for this sample to work

[UPM](https://github.com/intel-iot-devkit/upm) & [MRAA](https://github.com/intel-iot-devkit/mraa) libraries

## Setup
Follow the steps listed in the hardware requirements section.

## Notes

Mraa I/O operations require permissions to UNIX character devices and sysfs not commonly granted to normal users by default. To avoid permission issues you can run your application as root by killing the target tcf-agent and reconnecting to your board as root. The systemd journal/logfile will reflect this as mraa logs most things to syslog. More information is available [here](https://github.com/intel-iot-devkit/mraa/blob/master/docs/debugging.md).
