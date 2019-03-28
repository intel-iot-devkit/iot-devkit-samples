#!/usr/bin/env python
# (C) Copyright 2018, Intel Corporation
# SPDX-License-Identifier: MIT

import mraa
import time

# TODO Ensure you are running this sample on a Linux system with;
# * python-mraa bindings installed
# * Permissions to write to the gpio system, this often requires root

# The following should blink a LED/GPIO on your mraa supported board.
# 13 is used as the default with this sample but this may not be the LED on your board.
# When using: 
#  * Intel Joule - use pin 101
#  * Minnowboard Max - use pin 104
#  * IEI Tank - use pin 1

led = mraa.Gpio(13)

# Set the direction of the LED/GPIO to OUT:
led.dir(mraa.DIR_OUT)

while True:
    led.write(1)
    time.sleep(1)
    led.write(0)
    time.sleep(1)
