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

from __future__ import unicode_literals
from gnuradio import gr
from gnuradio import audio
from gnuradio.eng_arg import eng_float
from argparse import ArgumentParser

try:
    from gnuradio import analog
except ImportError:
    sys.stderr.write("Error: Program requires gr-analog.\n")
    sys.exit(1)

#import os
#print os.getpid()
#raw_input('Attach gdb and press Enter: ')

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-O", "--audio-output", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=48000,
                          help="set sample rate to RATE %(default)r)")
        parser.add_argument("-D", "--dont-block", action="store_false", default=True,
                          dest="ok_to_block")

        args = parser.parse_args()
        sample_rate = int(args.sample_rate)
        ampl = 0.5

        src0 = analog.sig_source_f(sample_rate, analog.GR_SIN_WAVE, 650, ampl)

        dst = audio.sink(sample_rate,
                         args.audio_output,
                         args.ok_to_block)

        self.connect (src0, (dst, 0))


if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
