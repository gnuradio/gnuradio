#!/usr/bin/env python
#
# Copyright 2003 Free Software Foundation, Inc.
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

import sys
import struct

fin = sys.stdin

count = 0
expected = 0
last_correction = 0

while 1:
    s = fin.read(2)
    if not s or len(s) != 2:
        break

    v, = struct.unpack ('H', s)
    iv = int(v) & 0xffff
    # print "%8d  %6d  0x%04x" % (count, iv, iv)
    if count & 0x1:                     # only counting on the Q channel
        if (expected & 0xffff) != iv:
            print "%8d  (%6d) %6d  0x%04x" % (count, count - last_correction, iv, iv)
            expected = iv               # reset expected sequence
            last_correction = count
        expected = (expected + 1) & 0xffff 
        
    count += 1

    


