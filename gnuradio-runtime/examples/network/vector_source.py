#!/usr/bin/env python
#
# Copyright 2006,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import blocks
from argparse import ArgumentParser


class vector_source(gr.top_block):
    def __init__(self, host, port, pkt_size, eof):
        gr.top_block.__init__(self, "vector_source")
        data = [i * 0.01 for i in range(1000)]
        vec = blocks.vector_source_f(data, True)
        udp = blocks.udp_sink(gr.sizeof_float, host, port, pkt_size, eof=eof)
        self.connect(vec, udp)


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1",
                        help="Remote host name (domain name or IP address")
    parser.add_argument("--port", type=int, default=65500,
                        help="port number to connect to")
    parser.add_argument("--packet-size", type=int, default=1471,
                        help="packet size.")
    parser.add_argument("--no-eof", action="store_true", default=False,
                        help="don't send EOF on disconnect")
    args = parser.parse_args()
# Create an instance of a hierarchical block
    top_block = vector_source(args.host, args.port, args.packet_size,
                              not args.no_eof)

    try:
        # Run forever
        top_block.run()
    except KeyboardInterrupt:
        # Ctrl-C exits
        pass
