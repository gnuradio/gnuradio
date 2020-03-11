#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2008,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

# GNU Radio example program to record a dial tone to a WAV file

from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import blocks
from gnuradio.eng_arg import eng_float
from argparse import ArgumentParser

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=48000,
                          help="set sample rate to RATE (%(default)r)")
        parser.add_argument("-N", "--samples", type=eng_float, required=True,
                          help="number of samples to record")
        parser.add_argument('file_name', metavar='WAV-FILE',
                          help='Output WAV file name', nargs=1)
        args = parser.parse_args()

        sample_rate = int(args.sample_rate)
        ampl = 0.1

        src0 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 350, ampl)
        src1 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 440, ampl)
        head0 = blocks.head(gr.sizeof_float, int(args.samples))
        head1 = blocks.head(gr.sizeof_float, int(args.samples))
        dst = blocks.wavfile_sink(args.file_name[0], 2, int(args.sample_rate), 16)

        self.connect(src0, head0, (dst, 0))
        self.connect(src1, head1, (dst, 1))

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
