#!/usr/bin/env python
#
# Copyright 2004,2005,2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

class dial_tone(gr.hier_block2):
    def __init__(self):
        gr.hier_block2.__init__(self, 
	                        "dial_tone", 
	                        gr.io_signature(0,0,0), 
				gr.io_signature(0,0,0))

        parser = OptionParser(option_class=eng_option)
        parser.add_option("-O", "--audio-output", type="string", default="",
                          help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
        parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
                          help="set sample rate to RATE (48000)")
        (options, args) = parser.parse_args ()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        sample_rate = int(options.sample_rate)
        ampl = 0.1

	self.define_component("source0", gr.sig_source_f (sample_rate, gr.GR_SIN_WAVE, 350, ampl))
	self.define_component("source1", gr.sig_source_f (sample_rate, gr.GR_SIN_WAVE, 440, ampl))
	self.define_component("dest", audio.sink(sample_rate, options.audio_output))
	self.connect("source0", 0, "dest", 0)	
	self.connect("source1", 0, "dest", 1)
	
if __name__ == '__main__':
    top_block = dial_tone()
    runtime = gr.runtime(top_block)

    try:    
        runtime.run()
    except KeyboardInterrupt:
        pass
