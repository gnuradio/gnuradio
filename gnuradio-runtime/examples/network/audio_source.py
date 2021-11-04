#!/usr/bin/env python
#
# Copyright 2006,2007,2010 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import blocks
from argparse import ArgumentParser
import sys

try:
    from gnuradio import audio
except ImportError:
    sys.stderr.write(
        "Failed to import gnuradio.audio. Make sure gr-audio component is installed.\n")
    sys.exit(1)


class audio_source(gr.top_block):
    def __init__(self, host, port, pkt_size, sample_rate, eof):
        gr.top_block.__init__(self, "audio_source")
        self.audio = audio.source(sample_rate)
        self.sink = blocks.udp_sink(
            gr.sizeof_float, host, port, pkt_size, eof=eof)
        self.connect(self.audio, self.sink)


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1",
                        help="Remote host name (domain name or IP address")
    parser.add_argument("--port", type=int, default=65500,
                        help="port number to connect to")
    parser.add_argument("--packet-size", type=int, default=1472,
                        help="packet size.")
    parser.add_argument("-r", "--sample-rate", type=int, default=32000,
                        help="audio signal sample rate [default=%(default)r]")
    parser.add_argument("--no-eof", action="store_true", default=False,
                        help="don't send EOF on disconnect")
    args = parser.parse_args()
    # Create an instance of a hierarchical block
    top_block = audio_source(args.host, args.port,
                             args.packet_size, args.sample_rate,
                             not args.no_eof)

    try:
        # Run forever
        top_block.run()
    except KeyboardInterrupt:
        # Ctrl-C exits
        pass
