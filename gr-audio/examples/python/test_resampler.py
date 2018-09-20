#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2012 Free Software Foundation, Inc.
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

from __future__ import print_function
from __future__ import division
from __future__ import unicode_literals
from gnuradio import gr, gru
from gnuradio import audio
from gnuradio import filter
from gnuradio.eng_arg import eng_float, intx
from argparse import ArgumentParser

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

try:
    from gnuradio import blocks
except ImportError:
    sys.stderr.write("Error: Program requires gr-blocks.\n")
    sys.exit(1)

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-O", "--audio-output", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-i", "--input-rate", type=eng_float, default=8000,
                          help="set input sample rate to RATE %(default)r")
        parser.add_argument("-o", "--output-rate", type=eng_float, default=48000,
                          help="set output sample rate to RATE %(default)r")
        args = parser.parse_args()
        input_rate = int(args.input_rate)
        output_rate = int(args.output_rate)

        interp = gru.lcm(input_rate / output_rate, input_rate)
        decim = gru.lcm(input_rate / output_rate, output_rate)

        print("interp =", interp)
        print("decim  =", decim)

        ampl = 0.1
        src0 = analog.sig_source_f(input_rate, analog.GR_SIN_WAVE, 650, ampl)
        rr = filter.rational_resampler_fff(interp, decim)
        dst = audio.sink(output_rate, args.audio_output)
        self.connect(src0, rr, (dst, 0))

if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
