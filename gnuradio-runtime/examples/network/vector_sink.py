#!/usr/bin/env python
#
# Copyright 2006,2010 Free Software Foundation, Inc.
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

from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import blocks
from gnuradio.eng_arg import eng_float, intx
from argparse import ArgumentParser

class vector_sink(gr.top_block):
    def __init__(self, host, port, pkt_size, eof):
        gr.top_block.__init__(self, "vector_sink")

        udp = blocks.udp_source(gr.sizeof_float, host, port, pkt_size, eof=eof)
        sink = blocks.file_sink(gr.sizeof_float, "received.dat")
        self.connect(udp, sink)

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("-H", "--host", default="0.0.0.0",
                      help="local host name (domain name or IP address)")
    parser.add_argument("-p", "--port", type=int, default=65500,
                      help="port value to listen to for connection")
    parser.add_argument("-s", "--packet-size", type=int, default=1471,
                      help="packet size.")
    parser.add_argument("--no-eof", action="store_true", default=False,
                      help="don't send EOF on disconnect")
    args = parser.parse_args()
    # Create an instance of a hierarchical block
    top_block = vector_sink(args.host, args.port,
                            args.packet_size,
                            not args.no_eof)

    try:
        # Run forever
        top_block.run()
    except KeyboardInterrupt:
        # Ctrl-C exits
        pass

