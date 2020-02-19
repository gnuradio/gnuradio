#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

#
# Gang - Here's a simple script that demonstrates spectrum inversion
# using the multiply by [1,-1] method (mixing with Nyquist frequency).
# Requires nothing but a sound card, and sounds just like listening
# to a SSB signal on the wrong sideband.
#

from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import audio
from gnuradio import blocks
from gnuradio.eng_arg import eng_float, intx
from argparse import ArgumentParser

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-I", "--audio-input", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-O", "--audio-output", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=8000,
                          help="set sample rate to RATE (%(default)r)")
        args = parser.parse_args()
        sample_rate = int(args.sample_rate)
        src = audio.source (sample_rate, args.audio_input)
        dst = audio.sink (sample_rate, args.audio_output)

        vec1 = [1, -1]
        vsource = blocks.vector_source_f(vec1, True)
        multiply = blocks.multiply_ff()

        self.connect(src, (multiply, 0))
        self.connect(vsource, (multiply, 1))
        self.connect(multiply, dst)


if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
