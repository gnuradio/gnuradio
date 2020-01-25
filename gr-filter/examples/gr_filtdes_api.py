#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function
from __future__ import unicode_literals
from gnuradio.filter import filter_design
import sys

'''
API Blocking call
returns filter taps for FIR filter design
returns b,a for IIR filter design
'''
filtobj = filter_design.launch(sys.argv)

# Displaying all filter parameters
print("Filter Count:", filtobj.get_filtercount())
print("Filter type:", filtobj.get_restype())
print("Filter params", filtobj.get_params())
print("Filter Coefficients", filtobj.get_taps())
