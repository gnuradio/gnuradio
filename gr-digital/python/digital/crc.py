#
# Copyright 2005,2007,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
"""
Backward-compatible wrapper for CRC32
"""

import zlib
import struct

def gen_and_append_crc32(data):
    """
    Return a binary string with its CRC appended
    """
    return data + struct.pack(">I", zlib.crc32(data))

def check_crc32(data):
    """
    Check if the byte string 's' has a valid CRC on its last four bytes

    Returns a tuple (is CRC valid, string-without-CRC).
    """
    if len(data) < 4:
        return (False, '')
    msg = data[:-4] # Message without the CRC
    actual = zlib.crc32(msg)
    (expected,) = struct.unpack(">I", data[-4:])
    return (actual == expected, msg)
