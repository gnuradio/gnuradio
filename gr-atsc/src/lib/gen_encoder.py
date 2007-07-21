#!/usr/bin/python
#
# Copyright 2002 Free Software Foundation, Inc.
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

def output(input, state):
    x2 = (input >> 1) & 0x1
    x1 = (input >> 0) & 0x1
    s = (state >> 2) & 0x1
    t = (state >> 1) & 0x1
    u = (state >> 0) & 0x1

    z0 = u
    z1 = x1
    z2 = x2 ^ s
    return (z2 << 2) | (z1 << 1) | z0


def next_state(input, state):
    x2 = (input >> 1) & 0x1
    x1 = (input >> 0) & 0x1
    s0 = (state >> 2) & 0x1
    t0 = (state >> 1) & 0x1
    u0 = (state >> 0) & 0x1

    s1 = x2 ^ s0
    t1 = u0
    u1 = t0 ^ x1
    
    return (s1 << 2) | (t1 << 1) | u1

print "@@@ NEXT @@@"

for i in range (32):
    state = (i >> 2) & 0x7
    input = i & 0x3
    print next_state (input, state)


print "@@@ OUTPUT @@@"

for i in range (32):
    state = (i >> 2) & 0x7
    input = i & 0x3
    print output (input, state)
    
