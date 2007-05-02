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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

import sys
import struct
from optparse import OptionParser

from usb_packet import *

def dump_packet(raw_pkt, outfile, dump_payload):
    pkt = usb_packet(raw_pkt)
    outfile.write(pkt.decoded_flags())
    outfile.write(' chan= %2d  len= %3d timestamp= 0x%08x rssi= % 2d  tag= %2d\n' % (
        pkt.chan(), pkt.payload_len(), pkt.timestamp(), pkt.rssi(), pkt.tag()))
    if dump_payload:
        assert pkt.payload_len() % 4 == 0
        shorts = struct.unpack('<%dh' % (pkt.payload_len() // 2), pkt.payload())
        for i in range(0, len(shorts), 2):
            outfile.write('  %6d, %6d\n' % (shorts[i], shorts[i+1]))
        

def dump_packets(infile, outfile, dump_payload):
    raw_pkt = infile.read(512)
    while raw_pkt:
        if len(raw_pkt) != 512:
            sys.stderr.write("File length is not a multiple of 512 bytes")
            raise SystemExit, 1

        dump_packet(raw_pkt, outfile, dump_payload)
        raw_pkt = infile.read(512)


def main():
    parser = OptionParser()
    parser.add_option('-p', '--dump-payload', action='store_true', default=False,
                      help='dump payload in decimal and hex')

    (options, files) = parser.parse_args()
    if len(files) == 0:
        dump_packets(sys.stdin, sys.stdout, options.dump_payload)
    else:
        for f in files:
            dump_packets(open(f, "r"), sys.stdout, options.dump_payload)


if __name__ == '__main__':
    main()
