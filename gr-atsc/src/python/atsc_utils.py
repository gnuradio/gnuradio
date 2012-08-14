#
# Copyright 2006 Free Software Foundation, Inc.
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

import random
import sys

MPEG_SYNC_BYTE = 0x47

def make_fake_transport_stream_packet(npkts):
    """
    Return a sequence of 8-bit ints that represents an MPEG Transport Stream packet.

    Args:
        npkts: how many 188-byte packets to return

    FYI, each ATSC Data Frame contains two Data Fields, each of which contains
    312 data segments.  Each transport stream packet maps to a data segment.
    """
    r = [0] * (npkts * 188)
    i = 0
    for j in range(npkts):
        r[i+0] = MPEG_SYNC_BYTE
        r[i+1] = random.randint(0, 127) # top bit (transport error bit) clear
        i = i + 2
        for n in range(186):
            r[i + n] = random.randint(0, 255)
        i = i + 186

    return r


def pad_stream(src, sizeof_total, sizeof_pad):
    sizeof_valid = sizeof_total - sizeof_pad
    assert sizeof_valid > 0
    assert (len(src) % sizeof_valid) == 0
    npkts = len(src) // sizeof_valid
    dst = [0] * (npkts * sizeof_total)
    for i in range(npkts):
        src_s = i * sizeof_valid
        dst_s = i * sizeof_total
        dst[dst_s:dst_s + sizeof_valid] = src[src_s:src_s + sizeof_valid]
    return dst


def depad_stream(src, sizeof_total, sizeof_pad):
    sizeof_valid = sizeof_total - sizeof_pad
    assert sizeof_valid > 0
    assert (len(src) % sizeof_total) == 0
    npkts = len(src) // sizeof_total
    dst = [0] * (npkts * sizeof_valid)
    for i in range(npkts):
        src_s = i * sizeof_total
        dst_s = i * sizeof_valid
        dst[dst_s:dst_s + sizeof_valid] = src[src_s:src_s + sizeof_valid]
    return dst


