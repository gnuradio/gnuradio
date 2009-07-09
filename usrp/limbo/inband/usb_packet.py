#
# Copyright 2007 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

import struct


FL_OVERRUN        = 0x80000000
FL_UNDERRUN       = 0x40000000
FL_DROPPED        = 0x20000000
FL_END_OF_BURST   = 0x10000000
FL_START_OF_BURST = 0x08000000

FL_ALL_FLAGS      = 0xf8000000

FL_OVERRUN_SHIFT = 31
FL_UNDERRUN_SHIFT = 30
FL_DROPPED_SHIFT = 29
FL_END_OF_BURST_SHIFT = 28
FL_START_OF_BURST_SHIFT = 27
  
RSSI_MASK = 0x3f
RSSI_SHIFT = 21

CHAN_MASK = 0x1f
CHAN_SHIFT = 16

TAG_MASK = 0xf
TAG_SHIFT = 9

PAYLOAD_LEN_MASK = 0x1ff
PAYLOAD_LEN_SHIFT = 0

def make_header(flags, chan, payload_len, timestamp, rssi=0, tag=0):
    word0 =  ((flags & FL_ALL_FLAGS)
              | ((rssi & RSSI_MASK) << RSSI_SHIFT)
              | ((chan & CHAN_MASK) << CHAN_SHIFT)
              | ((tag & TAG_MASK) << TAG_SHIFT)
              | ((payload_len & PAYLOAD_LEN_MASK) << PAYLOAD_LEN_SHIFT))
    word1 = timestamp
    return struct.pack('<2I', word0, word1)


def _decode(pred, indicator):
    if pred:
        return indicator
    else:
        return '-'


class usb_packet(object):
    def __init__(self, raw_pkt):
        assert isinstance(raw_pkt, str) and len(raw_pkt) == 512
        self._raw_pkt = raw_pkt;
        (self._word0, self._word1) = struct.unpack('<2I', self._raw_pkt[0:8])

    def timestamp(self):
        return self._word1

    def rssi(self):
        return (self._word0 >> RSSI_SHIFT) & RSSI_MASK

    def chan(self):
        return (self._word0 >> CHAN_SHIFT) & CHAN_MASK

    def tag(self):
        return (self._word0 >> TAG_SHIFT) & TAG_MASK

    def payload_len(self):
        return (self._word0 >> PAYLOAD_LEN_SHIFT) & PAYLOAD_LEN_MASK

    def flags(self):
        return self._word0 & FL_ALL_FLAGS

    def overrun(self):
        return (self._word0 >> FL_OVERRUN_SHIFT) & 0x1

    def underrun(self):
        return (self._word0 >> FL_UNDERRUN_SHIFT) & 0x1

    def start_of_burst(self):
        return (self._word0 >> FL_START_OF_BURST_SHIFT) & 0x1

    def end_of_burst(self):
        return (self._word0 >> FL_END_OF_BURST_SHIFT) & 0x1

    def dropped(self):
        return (self._word0 >> FL_DROPPED_SHIFT) & 0x1

    def payload(self):
        return self._raw_pkt[8:8+self.payload_len()]

    def decoded_flags(self):
        s = (_decode(self.overrun(), 'O')
             + _decode(self.underrun(), 'U')
             + _decode(self.dropped(), 'D')
             + _decode(self.end_of_burst(), 'E')
             + _decode(self.start_of_burst(), 'S'))
        return s
