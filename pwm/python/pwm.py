#!/usr/bin/env python
# (C) Copyright 2018, Intel Corporation
# SPDX-License-Identifier: MIT

import mraa
import time

# TODO Ensure you are running this sample on a Linux system with;
# * python-mraa bindings installed
# * Permissions to write to the pwm system, this often requires root

# Usage: Generates PWM at a step rate of 0.01 continuously.
# NOTE: Not all pins can be used as PWM, please refer to your Board's
# documention. PWM pins are often marked with a ~ on the board's silk screen

# Initilise PWM on pin 3
x = mraa.Pwm(3)

# set PWM period
x.period_us(700)

# enable PWM
x.enable(True)

value= 0.0

while True:
    # write PWM value
    x.write(value)

    time.sleep(0.05)

    value = value + 0.01
    if value >= 1:
        value = 0.0