#!/usr/bin/env python
# (C) Copyright 2018, Intel Corporation
# SPDX-License-Identifier: MIT

import mraa
import time

# TODO Ensure you are running this sample on a Linux system with;
# * python-mraa bindings installed
# * Permissions to write to the gpio system, this often requires root

# The following should read the level of the choosen GPIO Pin
# 13 is used as the default with this sample but this may not be usable on your board.
# When using: 
#  * Intel Joule - use pin 101
#  * Minnowboard Max - use pin 104
#  * IEI Tank - use pin 0

gpio = mraa.Gpio(13)

# Set the direction of the GPIO to IN:
gpio.dir(mraa.DIR_IN)

while True:
    print(gpio.read())
    time.sleep(1)