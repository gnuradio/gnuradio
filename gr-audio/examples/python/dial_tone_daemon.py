#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2008,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import print_function
from __future__ import unicode_literals
from gnuradio import gr, gru
from gnuradio import audio
from gnuradio.eng_arg import eng_float
from argparse import ArgumentParser
import os

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-O", "--audio-output", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=48000,
                          help="set sample rate to RATE (%(default)r)")
        args = parser.parse_args()
        sample_rate = int(args.sample_rate)
        ampl = 0.1

        src0 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 350, ampl)
        src1 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 440, ampl)
        dst = audio.sink(sample_rate, args.audio_output)
        self.connect(src0, (dst, 0))
        self.connect(src1, (dst, 1))

if __name__ == '__main__':
    pid = gru.daemonize()
    print("To stop this program, enter 'kill %d'" % pid)
    my_top_block().run()
