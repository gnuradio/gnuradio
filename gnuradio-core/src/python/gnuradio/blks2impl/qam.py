#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

from math import pi, sqrt
import math

# These constellations are generated for Gray coding when symbos [1, ..., m] are used
# Mapping to Gray coding is therefore unnecessary

def make_constellation(m):
    # number of bits/symbol (log2(M))
    k = int(math.log10(m) / math.log10(2.0))

    coeff = 1
    const_map = []
    for i in range(m):
        a = (i&(0x01 << k-1)) >> k-1
        b = (i&(0x01 << k-2)) >> k-2
        bits_i = [((i&(0x01 << k-j-1)) >> k-j-1) for j in range(2, k, 2)]
        bits_q = [((i&(0x01 << k-j-1)) >> k-j-1) for j in range(3, k, 2)]

        ss = 0
        ll = len(bits_i)
        for ii in range(ll):
            rr = 0
            for jj in range(ll-ii):
                rr = abs(bits_i[jj] - rr)
            ss += rr*pow(2.0, ii+1)
        re = (2*a-1)*(ss+1)
        
        ss = 0
        ll = len(bits_q)
        for ii in range(ll):
            rr = 0
            for jj in range(ll-ii):
                rr = abs(bits_q[jj] - rr)
            ss += rr*pow(2.0, ii+1)
        im = (2*b-1)*(ss+1)

        a = max(re, im)
        if a > coeff:
            coeff = a
        const_map.append(complex(re, im))

    norm_map = [complex(i.real/coeff, i.imag/coeff) for i in const_map]
    return norm_map
        
# Common definition of constellations for Tx and Rx
constellation = {
    4 :  make_constellation(4),           # QAM4 (QPSK)
    8 :  make_constellation(8),           # QAM8
    16:  make_constellation(16),          # QAM16
    64:  make_constellation(64),          # QAM64
    256: make_constellation(256)          # QAM256
    }

# -----------------------
# Do Gray code
# -----------------------
# binary to gray coding
binary_to_gray = {
    4 : range(4),
    8 : range(8),
    16: range(16),
    64: range(64),
    256: range(256)
    }
   
# gray to binary
gray_to_binary = {
    4 : range(4),
    8 : range(8),
    16: range(16),
    64: range(64),
    256: range(256)
    }

# -----------------------
# Don't Gray code
# -----------------------
# identity mapping
binary_to_ungray = {
    4 : range(4),
    8 : range(8),
    16: range(16),
    64: range(64)
    }
    
# identity mapping
ungray_to_binary = {
    4 : range(4),
    8 : range(8),
    16: range(16),
    64: range(64)
    }
