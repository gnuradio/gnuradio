#!/usr/bin/env python
#
# Copyright 2009,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import filter_design
import sys

'''
Callback example
Function called when "design" button is pressed
or pole-zero plot is changed
'''
def print_params(filtobj):
    print "Filter Count:", filtobj.get_filtercount()
    print "Filter type:", filtobj.get_restype()
    print "Filter params", filtobj.get_params()
    print "Filter Coefficients", filtobj.get_taps()

filter_design.launch(sys.argv, print_params)
