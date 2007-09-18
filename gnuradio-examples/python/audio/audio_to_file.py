#!/usr/bin/env python
#
# Copyright 2004,2007 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class my_top_block(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self)

        usage="%prog: [options] output_filename"
        parser = OptionParser(option_class=eng_option, usage=usage)
        parser.add_option("-I", "--audio-input", type="string", default="",
                          help="pcm input device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
                          help="set sample rate to RATE (48000)")
        parser.add_option("-N", "--nsamples", type="eng_float", default=None,
                          help="number of samples to collect [default=+inf]")
     
        (options, args) = parser.parse_args ()
        if len(args) != 1:
            parser.print_help()
            raise SystemExit, 1
        filename = args[0]

        sample_rate = int(options.sample_rate)
        src = audio.source (sample_rate, options.audio_input)
        dst = gr.file_sink (gr.sizeof_float, filename)

        if options.nsamples is None:
            self.connect((src, 0), dst)
        else:
            head = gr.head(gr.sizeof_float, int(options.nsamples))
            self.connect((src, 0), head, dst)


if __name__ == '__main__':
    try:
        my_top_block().run()
    except KeyboardInterrupt:
        pass
