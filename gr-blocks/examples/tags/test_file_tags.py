#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function
from __future__ import unicode_literals

from gnuradio import gr
from gnuradio import blocks
import sys
import numpy

def main():
    data = numpy.arange(0, 32000, 1).tolist()
    trig = 100*[0,] + 100*[1,]

    src = blocks.vector_source_s(data, True)
    trigger = blocks.vector_source_s(trig, True)

    thr = blocks.throttle(gr.sizeof_short, 10e3)
    ann = blocks.annotator_alltoall(1000000, gr.sizeof_short)
    tagger = blocks.burst_tagger(gr.sizeof_short)

    fsnk = blocks.tagged_file_sink(gr.sizeof_short, 1)

    tb = gr.top_block()
    tb.connect(src, thr, (tagger, 0))
    tb.connect(trigger, (tagger, 1))
    tb.connect(tagger, fsnk)

    tb.run()

if __name__ == "__main__":
    main()
