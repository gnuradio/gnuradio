#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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

import struct

def read_binary(filename, type):
    n = struct.calcsize(type)
    f = open(filename, 'rb')
    out = list()
    lin = f.read(n)
    while(len(lin) == n):
        cp = struct.unpack(type, lin)
        out.append(cp)
        lin = f.read(n)
    return out

def read_char_binary(filename):
    return read_binary(filename, 'c')

def read_float_binary(filename):
    return read_binary(filename, 'f')

def read_int_binary(filename):
    return read_binary(filename, 'i')

def read_short_binary(filename):
    return read_binary(filename, 'h')

def read_complex_binary(filename):
    n = struct.calcsize('ff')
    f = open(filename, 'rb')
    re = list()
    im = list()
    lin = f.read(n)
    while(len(lin) == n):
        cp = struct.unpack('ff', lin)
        re.append(cp[0])
        im.append(cp[1])
        lin = f.read(n)
    return (re, im)


