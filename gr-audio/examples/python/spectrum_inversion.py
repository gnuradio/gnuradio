#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2013 Free Software Foundation, Inc.
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
# Gang - Here's a simple script that demonstrates spectrum inversion
# using the multiply by [1,-1] method (mixing with Nyquist frequency).
# Requires nothing but a sound card, and sounds just like listening
# to a SSB signal on the wrong sideband.
#

from gnuradio import gr
from gnuradio import audio
from gnuradio import blocks
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-I", "--audio-input", type="string", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=8000,
                          help="set sample rate to RATE (8000)")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
        src = audio.source (sample_rate, options.audio_input)
        dst = audio.sink (sample_rate, options.audio_output)

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
