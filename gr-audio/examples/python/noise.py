#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import audio
from gnuradio import digital
from gnuradio.eng_arg import eng_float
from argparse import ArgumentParser

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-O", "--audio-output", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=48000,
                          help="set sample rate to RATE (48000)")
        args = parser.parse_args()
        sample_rate = int(args.sample_rate)
        ampl = 0.1

        src = digital.glfsr_source_b(32)     # Pseudorandom noise source
        b2f = digital.chunks_to_symbols_bf([ampl, -ampl], 1)
        dst = audio.sink(sample_rate, args.audio_output)
        self.connect(src, b2f, dst)

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
