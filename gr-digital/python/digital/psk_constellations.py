#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

import numpy
from constellation_map_generator import constellation_map_generator

'''
Note on the naming scheme. Each constellation is named using a prefix
for the type of constellation, the order of the constellation, and a
distinguishing feature, which comes in three modes:

- No extra feature: the basic Gray-coded constellation map; others
  will be derived from this type.
- A single number: an indexed number to uniquely identify different
  constellation maps.
- 0xN_x0_x1..._xM: A permutation of the base constellation, explained
  below.

For rectangular constellations (BPSK, QPSK, QAM), we can define a
hyperspace and look for all symmetries. This is also known as the
automorphism group of the hypercube, aka the hyperoctahedral
group. What this means is that we can easily define all possible
rotations in terms of the first base mapping by creating the mapping:

  f(x) = k XOR pi(x)

The x is the bit string for the symbol we are altering. Then k is a
bit string of n bits where n is the number of bits per symbol in the
constellation (e.g., 2 for QPSK or 6 for QAM64). The pi is a
permutation function specified as pi_0, pi_1..., pi_n-1. This permutes
the bits from the base constellation symbol to a new code, which is
then xor'd by k.

The value of k is from 0 to 2^n-1 and pi is a list of all bit
positions.

The total number of Gray coded modulations is (2^n)*(n!).

We create aliases for all possible naming schemes for the
constellations. So if a hyperoctahedral group is defined, we also set
this function equal to a function name using a unique ID number, and
we always select one rotation as our basic rotation that the other
rotations are based off of.
'''

# BPSK Constellation Mappings

def psk_2_0x0():
    '''
    0 | 1
    '''
    const_points = [-1, 1]
    symbols = [0, 1]
    return (const_points, symbols)
psk_2 = psk_2_0x0  # Basic BPSK rotation
psk_2_0 = psk_2    # First ID for BPSK rotations

def psk_2_0x1():
    '''
    1 | 0
    '''
    const_points = [-1, 1]
    symbols = [1, 0]
    return (const_points, symbols)
psk_2_1 = psk_2_0x1


############################################################
# BPSK Soft bit LUT generators
############################################################

def sd_psk_2_0x0(x, Es=1):
    '''
    0 | 1
    '''
    x_re = x.real
    dist = Es*numpy.sqrt(2)
    return [dist*x_re,]
sd_psk_2 = sd_psk_2_0x0  # Basic BPSK rotation
sd_psk_2_0 = sd_psk_2    # First ID for BPSK rotations

def sd_psk_2_0x1(x, Es=1):
    '''
    1 | 0
    '''
    x_re = [x.real,]
    dist = Es*numpy.sqrt(2)
    return -dist*x_re
sd_psk_2_1 = sd_psk_2_0x1


############################################################
# QPSK Constellation Mappings
############################################################

def psk_4_0x0_0_1():
    '''
    | 10 | 11
    | -------
    | 00 | 01
    '''
    const_points = [-1-1j, 1-1j,
                    -1+1j, 1+1j]
    symbols = [0, 1, 2, 3]
    return (const_points, symbols)
psk_4 = psk_4_0x0_0_1
psk_4_0 = psk_4

def psk_4_0x1_0_1():
    '''
    | 11 | 10
    | -------
    | 01 | 00
    '''
    k = 0x1
    pi = [0, 1]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_1 = psk_4_0x1_0_1

def psk_4_0x2_0_1():
    '''
    | 00 | 01
    | -------
    | 10 | 11
    '''
    k = 0x2
    pi = [0, 1]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_2 = psk_4_0x2_0_1

def psk_4_0x3_0_1():
    '''
    | 01 | 00
    | -------
    | 11 | 10
    '''
    k = 0x3
    pi = [0, 1]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_3 = psk_4_0x3_0_1

def psk_4_0x0_1_0():
    '''
    | 01 | 11
    | -------
    | 00 | 10
    '''
    k = 0x0
    pi = [1, 0]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_4 = psk_4_0x0_1_0

def psk_4_0x1_1_0():
    '''
    | 00 | 10
    | -------
    | 01 | 11
    '''
    k = 0x1
    pi = [1, 0]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_5 = psk_4_0x1_1_0

def psk_4_0x2_1_0():
    '''
    | 11 | 01
    | -------
    | 10 | 00
    '''
    k = 0x2
    pi = [1, 0]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_6 = psk_4_0x2_1_0

def psk_4_0x3_1_0():
    '''
    | 10 | 00
    | -------
    | 11 | 01
    '''
    k = 0x3
    pi = [1, 0]
    return constellation_map_generator(psk_4()[0], psk_4()[1], k, pi)
psk_4_7 = psk_4_0x3_1_0



############################################################
# QPSK Constellation Softbit LUT generators
############################################################

def sd_psk_4_0x0_0_1(x, Es=1):
    '''
    | 10 | 11
    | -------
    | 00 | 01
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [dist*x_im, dist*x_re]
sd_psk_4 = sd_psk_4_0x0_0_1
sd_psk_4_0 = sd_psk_4

def sd_psk_4_0x1_0_1(x, Es=1):
    '''
    | 11 | 10
    | -------
    | 01 | 00
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [dist*x_im, -dist*x_re]
sd_psk_4_1 = sd_psk_4_0x1_0_1

def sd_psk_4_0x2_0_1(x, Es=1):
    '''
    | 00 | 01
    | -------
    | 10 | 11
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [-dist*x_im, dist*x_re]
sd_psk_4_2 = sd_psk_4_0x2_0_1

def sd_psk_4_0x3_0_1(x, Es=1):
    '''
    | 01 | 00
    | -------
    | 11 | 10
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [-dist*x_im, -dist*x_re]
sd_psk_4_3 = sd_psk_4_0x3_0_1

def sd_psk_4_0x0_1_0(x, Es=1):
    '''
    | 01 | 11
    | -------
    | 00 | 10
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [dist*x_re, dist*x_im]
sd_psk_4_4 = sd_psk_4_0x0_1_0

def sd_psk_4_0x1_1_0(x, Es=1):
    '''
    | 00 | 10
    | -------
    | 01 | 11
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [dist*x_re, -dist*x_im]
sd_psk_4_5 = sd_psk_4_0x1_1_0


def sd_psk_4_0x2_1_0(x, Es=1):
    '''
    | 11 | 01
    | -------
    | 10 | 00
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [-dist*x_re, dist*x_im]
sd_psk_4_6 = sd_psk_4_0x2_1_0

def sd_psk_4_0x3_1_0(x, Es=1):
    '''
    | 10 | 00
    | -------
    | 11 | 01
    '''
    x_re = x.real
    x_im = x.imag
    dist = Es*numpy.sqrt(2)
    return [-dist*x_re, -dist*x_im]
sd_psk_4_7 = sd_psk_4_0x3_1_0
