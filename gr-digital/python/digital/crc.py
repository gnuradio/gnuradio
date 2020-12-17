#
# Copyright 2005,2007,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from . import digital_python as digital
import struct

def gen_and_append_crc32(s):
    crc = digital.crc32(s)
    return s + struct.pack(">I", crc)

def check_crc32(s):
    if len(s) < 4:
        return (False, '')
    msg = s[:-4]
    #print "msg = '%s'" % (msg,)
    actual = digital.crc32(msg)
    (expected,) = struct.unpack(">I", s[-4:])
    # print "actual =", hex(actual), "expected =", hex(expected)
    return (actual == expected, msg)
