#
# Copyright 2005,2007,2011 Free Software Foundation, Inc.
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

from gnuradio import gru
import digital_swig as digital
import struct

def gen_and_append_crc32(s):
    crc = digital.crc32(s)
    return s + struct.pack(">I", gru.hexint(crc) & 0xFFFFFFFF)

def check_crc32(s):
    if len(s) < 4:
        return (False, '')
    msg = s[:-4]
    #print "msg = '%s'" % (msg,)
    actual = digital.crc32(msg)
    (expected,) = struct.unpack(">I", s[-4:])
    # print "actual =", hex(actual), "expected =", hex(expected)
    return (actual == expected, msg)
