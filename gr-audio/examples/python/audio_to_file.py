#!/usr/bin/env python
#
# Copyright 2004,2007,2013 Free Software Foundation, Inc.
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
from gnuradio import blocks
from gnuradio.eng_arg import eng_float
from argparse import ArgumentParser

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = ArgumentParser()
        parser.add_argument("-I", "--audio-input", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_argument("-r", "--sample-rate", type=eng_float, default=48000,
                          help="set sample rate to RATE (%(default)r)")
        parser.add_argument("-N", "--nsamples", type=eng_float,
                          help="number of samples to collect [default=+inf]")
        parser.add_argument('file_name', metavar='FILE-NAME',
                          help="Output file path")

        args = parser.parse_args()

        sample_rate = int(args.sample_rate)
        src = audio.source(sample_rate, args.audio_input)
        dst = blocks.file_sink(gr.sizeof_float, args.file_name)

        if args.nsamples is None:
            self.connect((src, 0), dst)
        else:
            head = blocks.head(gr.sizeof_float, int(args.nsamples))
            self.connect((src, 0), head, dst)


if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
