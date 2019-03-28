#!/usr/bin/env python
# (C) Copyright 2018, Intel Corporation
# SPDX-License-Identifier: MIT

import mraa
import time

# TODO Ensure you are running this sample on a Linux system with;
# * python-mraa bindings installed
# * Permissions to write to the aio system, this often requires root

# The following should read a ADC/AIO value from your mraa supported board.
# 2 is used as the default with this sample but this may not a the AIO on your board.
# NOTE: Not all boards *have* an AIO/ADC please see your boards documention for details.

aio = mraa.Aio(2)

while True:
    print(aio.read())
