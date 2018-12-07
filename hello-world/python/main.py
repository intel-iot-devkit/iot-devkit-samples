#!/usr/bin/env python3
# (C) Copyright 2018, Intel Corporation
# SPDX-License-Identifier: MIT
import os

extra = os.getenv("HW", "Nothing")
print("Hello World! I see " + extra)
