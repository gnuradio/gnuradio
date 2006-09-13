#!/usr/bin/env python

# Copyright 2006 Free Software Foundation, Inc.
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

from gnuradio import gr, audio, eng_option
from gnuradio.eng_option import eng_option
from math import pi, cos
from optparse import OptionParser

"""
This script generates a standard dial tone and then applies a sinusoidal
envelope to vary it's loudness.  The audio is then passed through the
power squelch block before it gets sent to the sound card. By varying
the command line parameters, one can see the effect of differing
amounts of power averaging, threshold, and attack/decay ramping.
"""

class app_flow_graph(gr.flow_graph):
    def __init__(self, options, args):
        gr.flow_graph.__init__(self)
	
	# Create dial tone by adding two sine waves
	SRC1 = gr.sig_source_f(options.rate, gr.GR_SIN_WAVE, 350, 0.5, 0.0)
	SRC2 = gr.sig_source_f(options.rate, gr.GR_SIN_WAVE, 440, 0.5, 0.0)
	ADD = gr.add_ff()

	# Convert to vector stream (and back) to apply raised cosine envelope
	# You could also do this with a vector_source_f block that repeats.
	S2V = gr.stream_to_vector(gr.sizeof_float, options.rate)
	ENV = [0.5-cos(2*pi*x/options.rate)/2 for x in range(options.rate)]
	MLT = gr.multiply_const_vff(ENV)
	V2S = gr.vector_to_stream(gr.sizeof_float, options.rate)

	# Run through power squelch with user supplied or default options
	# Zero output when squelch is invoked
	SQL = gr.pwr_squelch_ff(options.threshold, options.alpha, options.ramp, False)
	DST = audio.sink(options.rate)

	# Solder it all together
	self.connect(SRC1, (ADD, 0))
	self.connect(SRC2, (ADD, 1))
	self.connect(ADD, S2V, MLT, V2S, SQL, DST)
	
def main():
    parser = OptionParser(option_class=eng_option)
    parser.add_option("-r", "--rate", type="int", default=8000, help="set audio output sample rate to RATE", metavar="RATE")
    parser.add_option("-t", "--threshold", type="eng_float", default=-10.0, help="set power squelch to DB", metavar="DB")
    parser.add_option("-a", "--alpha", type="eng_float", default=None, help="set alpha to ALPHA", metavar="ALPHA")
    parser.add_option("-m", "--ramp", type="int", default=None, help="set attack/decay ramp to SAMPLES", metavar="SAMPLES")
    (options, args) = parser.parse_args()

    if options.alpha == None:
	options.alpha = 50.0/options.rate
	
    if options.ramp == None:
	options.ramp = options.rate/50	# ~ 20 ms

    print "Using audio rate of", options.rate
    print "Using threshold of", options.threshold, "db"
    print "Using alpha of", options.alpha
    print "Using ramp of", options.ramp, "samples"

    fg = app_flow_graph(options, args)

    try:
      fg.run()
    except KeyboardInterrupt:
      pass

if __name__ == "__main__":
    main()
