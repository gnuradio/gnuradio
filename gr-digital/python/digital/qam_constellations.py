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

The permutation are given for b0_b1_b2_... for the total number of
bits. In the constellation diagrams shown in the comments, the bit
ordering is shown as [b3b2b1b0]. Bit values returned from the soft bit
LUTs are in the order [b3, b2, b1, b0].


The total number of Gray coded modulations is (2^n)*(n!).

We create aliases for all possible naming schemes for the
constellations. So if a hyperoctahedral group is defined, we also set
this function equal to a function name using a unique ID number, and
we always select one rotation as our basic rotation that the other
rotations are based off of.

For 16QAM:
 - n = 4
 - (2^n)*(n!) = 384
 - k \in [0x0, 0xF]
 - pi = 0, 1, 2, 3
        0, 1, 3, 2
        0, 2, 1, 3
        0, 2, 3, 1
        0, 3, 1, 2
        0, 3, 2, 1
        1, 0, 2, 3
        1, 0, 3, 2
        1, 2, 0, 3
        1, 2, 3, 0
        1, 3, 0, 2
        1, 3, 2, 0
        2, 0, 1, 3
        2, 0, 3, 1
        2, 1, 0, 3
        2, 1, 3, 0
        2, 3, 0, 1
        2, 3, 1, 0
        3, 0, 1, 2
        3, 0, 2, 1
        3, 1, 0, 2
        3, 1, 2, 0
        3, 2, 0, 1
        3, 2, 1, 0
'''

def qam_16_0x0_0_1_2_3():
    '''
    | 0010  0110 | 1110  1010
    |
    | 0011  0111 | 1111  1011
    | -----------------------
    | 0001  0101 | 1101  1001
    |
    | 0000  0100 | 1100  1000
    '''
    const_points = [-3-3j, -1-3j, 1-3j, 3-3j,
                    -3-1j, -1-1j, 1-1j, 3-1j,
                    -3+1j, -1+1j, 1+1j, 3+1j,
                    -3+3j, -1+3j, 1+3j, 3+3j]
    symbols = [0x0, 0x4, 0xC, 0x8,
               0x1, 0x5, 0xD, 0x9,
               0x3, 0x7, 0xF, 0xB,
               0x2, 0x6, 0xE, 0xA]
    return (const_points, symbols)
qam_16 = qam_16_0x0_0_1_2_3
qam_16_0 = qam_16

def qam_16_0x1_0_1_2_3():
    '''
    | 0011  0111 | 1111  1011
    |
    | 0010  0110 | 1110  1010
    | -----------------------
    | 0000  0100 | 1100  1000
    |
    | 0001  0101 | 1101  1001
    '''
    k = 0x1
    pi = [0, 1, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_1 = qam_16_0x1_0_1_2_3

def qam_16_0x2_0_1_2_3():
    '''
    | 0000  0100 | 1100  1000
    |
    | 0001  0101 | 1101  1001
    | -----------------------
    | 0011  0111 | 1111  1011
    |
    | 0010  0110 | 1110  1010
    '''
    k = 0x2
    pi = [0, 1, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_2 = qam_16_0x2_0_1_2_3

def qam_16_0x3_0_1_2_3():
    '''
    | 0001  0101 | 1101  1001
    |
    | 0000  0100 | 1100  1000
    | -----------------------
    | 0010  0110 | 1110  1010
    |
    | 0011  0111 | 1111  1011
    '''
    k = 0x3
    pi = [0, 1, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_3 = qam_16_0x3_0_1_2_3


def qam_16_0x0_1_0_2_3():
    '''
    | 0001  0101 | 1101  1001
    |
    | 0011  0111 | 1111  1011
    | -----------------------
    | 0010  0110 | 1110  1010
    |
    | 0000  0100 | 1100  1000
    '''
    k = 0x0
    pi = [1, 0, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_4 = qam_16_0x0_1_0_2_3

def qam_16_0x1_1_0_2_3():
    '''
    | 0000  0100 | 1100  1000
    |
    | 0010  0110 | 1110  1010
    | -----------------------
    | 0011  0111 | 1111  1011
    |
    | 0001  0101 | 1101  1001
    '''
    k = 0x1
    pi = [1, 0, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_5 = qam_16_0x1_1_0_2_3

def qam_16_0x2_1_0_2_3():
    '''
    | 0011  0111 | 1111  1011
    |
    | 0001  0101 | 1101  1001
    | -----------------------
    | 0000  0100 | 1100  1000
    |
    | 0010  0110 | 1110  1010
    '''
    k = 0x2
    pi = [1, 0, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_6 = qam_16_0x2_1_0_2_3

def qam_16_0x3_1_0_2_3():
    '''
    | 0010  0110 | 1110  1010
    |
    | 0000  0100 | 1100  1000
    | -----------------------
    | 0001  0101 | 1101  1001
    |
    | 0011  0111 | 1111  1011
    '''
    k = 0x3
    pi = [1, 0, 2, 3]
    return constellation_map_generator(qam_16()[0], qam_16()[1], k, pi)
qam_16_7 = qam_16_0x3_1_0_2_3


# Soft bit LUT generators

def sd_qam_16_0x0_0_1_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0010  0110 | 1110  1010
    |
    | 0011  0111 | 1111  1011
    | -----------------------
    | 0001  0101 | 1101  1001
    |
    | 0000  0100 | 1100  1000
    '''

    dist = Es*numpy.sqrt(2)
    boundary = dist/3.0
    dist0 = dist/6.0
#    print "Sample:    ", x
#    print "Es:        ", Es
#    print "Distance:  ", dist
#    print "Boundary:  ", boundary
#    print "1st Bound: ", dist0

    x_re = x.real
    x_im = x.imag

    if x_re < -boundary:
        b3 = boundary*(x_re + dist0)
    elif x_re < boundary:
        b3 = x_re
    else:
        b3 = boundary*(x_re - dist0)

    if x_im < -boundary:
        b1 = boundary*(x_im + dist0)
    elif x_im < boundary:
        b1 = x_im
    else:
        b1 = boundary*(x_im - dist0)

    b2 = -abs(x_re) + boundary
    b0 = -abs(x_im) + boundary

    return [(Es/2.0)*b3, (Es/2.0)*b2, (Es/2.0)*b1, (Es/2.0)*b0]
sd_qam_16 = sd_qam_16_0x0_0_1_2_3
sd_qam_16_0 = sd_qam_16

def sd_qam_16_0x1_0_1_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0011  0111 | 1111  1011
    |
    | 0010  0110 | 1110  1010
    | -----------------------
    | 0000  0100 | 1100  1000
    |
    | 0001  0101 | 1101  1001
    '''
    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b1 = 2*(x_im + 1)
    elif x_im < 2:
        b1 = x_im
    else:
        b1 = 2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b0 = +abs(x_im) - 2

    return [b3, b2, b1, b0]
sd_qam_16_1 = sd_qam_16_0x1_0_1_2_3

def sd_qam_16_0x2_0_1_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0000  0100 | 1100  1000
    |
    | 0001  0101 | 1101  1001
    | -----------------------
    | 0011  0111 | 1111  1011
    |
    | 0010  0110 | 1110  1010
    '''

    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b1 = -2*(x_im + 1)
    elif x_im < 2:
        b1 = -x_im
    else:
        b1 = -2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b0 = -abs(x_im) + 2

    return [b3, b2, b1, b0]
sd_qam_16_2 = sd_qam_16_0x2_0_1_2_3

def sd_qam_16_0x3_0_1_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0001  0101 | 1101  1001
    |
    | 0000  0100 | 1100  1000
    | -----------------------
    | 0010  0110 | 1110  1010
    |
    | 0011  0111 | 1111  1011
    '''
    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b1 = -2*(x_im + 1)
    elif x_im < 2:
        b1 = -x_im
    else:
        b1 = -2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b0 = +abs(x_im) - 2

    return [b3, b2, b1, b0]
sd_qam_16_3 = sd_qam_16_0x3_0_1_2_3

def sd_qam_16_0x0_1_0_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0001  0101 | 1101  1001
    |
    | 0011  0111 | 1111  1011
    | -----------------------
    | 0010  0110 | 1110  1010
    |
    | 0000  0100 | 1100  1000
    '''
    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b0 = 2*(x_im + 1)
    elif x_im < 2:
        b0 = x_im
    else:
        b0 = 2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b1 = -abs(x_im) + 2

    return [b3, b2, b1, b0]
sd_qam_16_4 = sd_qam_16_0x0_1_0_2_3

def sd_qam_16_0x1_1_0_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0000  0100 | 1100  1000
    |
    | 0010  0110 | 1110  1010
    | -----------------------
    | 0011  0111 | 1111  1011
    |
    | 0001  0101 | 1101  1001
    '''
    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b0 = -2*(x_im + 1)
    elif x_im < 2:
        b0 = -x_im
    else:
        b0 = -2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b1 = -abs(x_im) + 2

    return [b3, b2, b1, b0]
sd_qam_16_5 = sd_qam_16_0x1_1_0_2_3

def sd_qam_16_0x2_1_0_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0011  0111 | 1111  1011
    |
    | 0001  0101 | 1101  1001
    | -----------------------
    | 0000  0100 | 1100  1000
    |
    | 0010  0110 | 1110  1010
    '''
    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b0 = 2*(x_im + 1)
    elif x_im < 2:
        b0 = x_im
    else:
        b0 = 2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b1 = +abs(x_im) - 2

    return [b3, b2, b1, b0]
sd_qam_16_6 = sd_qam_16_0x2_1_0_2_3

def sd_qam_16_0x3_1_0_2_3(x, Es=1):
    '''
    | Soft bit LUT generator for constellation:
    |
    | 0010  0110 | 1110  1010
    |
    | 0000  0100 | 1100  1000
    | -----------------------
    | 0001  0101 | 1101  1001
    |
    | 0011  0111 | 1111  1011
    '''
    x_re = 3*x.real
    x_im = 3*x.imag

    if x_re < -2:
        b3 = 2*(x_re + 1)
    elif x_re < 2:
        b3 = x_re
    else:
        b3 = 2*(x_re - 1)

    if x_im < -2:
        b0 = -2*(x_im + 1)
    elif x_im < 2:
        b0 = -x_im
    else:
        b0 = -2*(x_im - 1)

    b2 = -abs(x_re) + 2
    b1 = +abs(x_im) - 2

    return [b3, b2, b1, b0]
sd_qam_16_7 = sd_qam_16_0x3_1_0_2_3
