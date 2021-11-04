#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
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


class my_graph(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        args = parser.parse_args()
        src = blocks.lfsr_32k_source_s()
        head = blocks.head(gr.sizeof_short, 2048)
        self.dst = blocks.vector_sink_s()
        self.connect(src, head, self.dst)


if __name__ == '__main__':
    try:
        tb = my_graph()
        tb.run()
        f = sys.stdout
        i = 0
        for s in tb.dst.data():
            f.write("%3d, " % (s & 0xff,))
            f.write("%3d, " % ((s >> 8) & 0xff,))
            i = i + 2
            if i % 16 == 0:
                f.write('\n')

    except KeyboardInterrupt:
        pass
