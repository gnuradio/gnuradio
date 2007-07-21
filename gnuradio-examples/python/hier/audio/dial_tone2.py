#!/usr/bin/env python
#
# Copyright 2004,2005,2006 Free Software Foundation, Inc.
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

# Top-level block creating a dial tone
# Derives from new class gr.hier_block2
class dial_tone(gr.top_block):
	def __init__(self, 
		     sample_rate,  # Audio output sample rate (int)
		     audio_output, # Audio output device
		     amplitude):   # Output volume (0.0-1.0)

		gr.top_block.__init__(self, "dial_tone")

		src0 = gr.sig_source_f(sample_rate, gr.GR_SIN_WAVE, 350, amplitude)
		src1 = gr.sig_source_f(sample_rate, gr.GR_SIN_WAVE, 440, amplitude)
		dst  = audio.sink(sample_rate, audio_output)

		self.connect(src0, (dst, 0))	
		self.connect(src1, (dst, 1))
	
if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option)
	parser.add_option("-O", "--audio-output", type="string", default="",
			  help="pcm output device name.  E.g., hw:0,0 or /dev/dsp")
	parser.add_option("-r", "--sample-rate", type="eng_float", default=48000,
			  help="set sample rate to RATE (48000)")
	parser.add_option("-a", "--amplitude", type="eng_float", default=0.1,
			  help="set output volume to AMPLITUDE (0.1)")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

	# Create an instance of a hierarchical block
	top = dial_tone(int(options.sample_rate), 
			options.audio_output, 
			options.amplitude)
			      
	# Create an instance of a runtime, passing it the top block
	# to process
	runtime = gr.runtime(top)

	try:    
		# Run forever
		runtime.run()
	except KeyboardInterrupt:
		# Ctrl-C exits
		pass
