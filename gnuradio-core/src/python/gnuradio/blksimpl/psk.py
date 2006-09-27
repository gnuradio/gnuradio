#
# Copyright 2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

from math import pi, sqrt
import cmath

def make_constellation(m):
    return [cmath.exp(i * 2 * pi / m * 1j) for i in range(m)]
        
# Common definition of constellations for Tx and Rx
constellation = {
    2 : make_constellation(2),           # BPSK
    4 : make_constellation(4),           # QPSK
    8 : make_constellation(8)            # 8PSK
    }

# -----------------------
# Do Gray code
# -----------------------
# binary to gray coding
binary_to_gray = {
    2 : (0, 1),
    4 : (0, 1, 3, 2),
    8 : (0, 1, 3, 2, 7, 6, 4, 5)
    }
   
# gray to binary
gray_to_binary = {
    2 : (0, 1),
    4 : (0, 1, 3, 2),
    8 : (0, 1, 3, 2, 6, 7, 5, 4)
    }

# -----------------------
# Don't Gray code
# -----------------------
# identity mapping
binary_to_ungray = {
    2 : (0, 1),
    4 : (0, 1, 2, 3),
    8 : (0, 1, 2, 3, 4, 5, 6, 7)
    }
    
# identity mapping
ungray_to_binary = {
    2 : (0, 1),
    4 : (0, 1, 2, 3),
    8 : (0, 1, 2, 3, 4, 5, 6, 7)
    }
