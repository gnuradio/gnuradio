#!/usr/bin/env /usr/bin/python
#
# Copyright 2004, 2013 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#
# This module starts the atsc processing chain taking the captured
# off-air signal created with:
#
#  uhd_rx_cfile.py  --samp-rate=6.4e6
#                   -f <center of tv signal channel freq>
#                   -g <appropriate gain for best signal / noise>
#                   -s output shorts
#
# This python script converts from interleaved shorts to the complex data type,
# then multiplies the sample rate by 3, from 6.4e6 to 19.2e6
# complex samples / sec, then lowpass filters with a cutoff of 3.2MHz
# and a transition band width of .5MHz.  Center of the tv channels is
# now at 0 with edges at -3.2MHz and 3.2MHz. This puts the pilot at
# -3MHz + 309KHz. Next a root raised cosine filter is applied to match the one
# in the transmitter and thus reduce ISI. The phased locked loop then locks to
# the pilot and outputs just the real part of the signal ( as information is
# not stored in the phase with atsc ), this is then feed to the bit lock
# loop, this looks for the bit sync marker put at the beginning of every segment
# field, this then adjusts the timing so the amplitude will be sampled at the
# correct sample ( sub-sample is used in this case ).
#
# Output is float.

from gnuradio import gr, analog, atsc
from gnuradio import blocks
from gnuradio import filter
import sys, math, os

def graph (args):

	print os.getpid()

	nargs = len(args)
    	if nargs == 2:
		infile = args[0]
        	outfile = args[1]
    	else:
        	raise ValueError('usage: interp.py input_file output_file.ts\n')

	input_rate = 19.2e6
	IF_freq = 5.75e6

	tb = gr.top_block()

	# Read from input file
	srcf = blocks.file_source(gr.sizeof_short, infile)

	# Convert interleaved shorts (I,Q,I,Q) to complex
	is2c = blocks.interleaved_short_to_complex()

	# 1/2 as wide because we're designing lp filter
	symbol_rate = atsc.ATSC_SYMBOL_RATE/2.
	NTAPS = 279
	tt = filter.firdes.root_raised_cosine (1.0, input_rate / 3, symbol_rate, .1152, NTAPS)
	rrc = filter.fir_filter_ccf(1, tt)

	# Interpolate Filter our 6MHz wide signal centered at 0
	ilp_coeffs = filter.firdes.low_pass(1, input_rate, 3.2e6, .5e6, filter.firdes.WIN_HAMMING)
	ilp = filter.interp_fir_filter_ccf(3, ilp_coeffs)

	# Move the center frequency to 5.75MHz ( this won't be needed soon )
	duc_coeffs = filter.firdes.low_pass ( 1, 19.2e6, 9e6, 1e6, filter.firdes.WIN_HAMMING )
    	duc = filter.freq_xlating_fir_filter_ccf ( 1, duc_coeffs, -5.75e6, 19.2e6 )

	# fpll input is float
	c2f = blocks.complex_to_float()

	# Phase locked loop
	fpll = atsc.fpll()

	# Clean fpll output
	lp_coeffs2 = filter.firdes.low_pass (1.0,
			   input_rate,
			   5.75e6,
                           120e3,
                           filter.firdes.WIN_HAMMING);
	lp_filter = filter.fir_filter_fff (1, lp_coeffs2)

	# Remove pilot ( at DC now )
	iir = filter.single_pole_iir_filter_ff(1e-5)
	remove_dc = blocks.sub_ff()

	# Bit Timing Loop, Field Sync Checker and Equalizer
	btl = atsc.bit_timing_loop()
	fsc = atsc.fs_checker()
	eq = atsc.equalizer()
	fsd = atsc.field_sync_demux()

	# Viterbi
	viterbi = atsc.viterbi_decoder()
        deinter = atsc.deinterleaver()
        rs_dec = atsc.rs_decoder()
        derand = atsc.derandomizer()
	depad = atsc.depad()

	# Write to output file
	outf = blocks.file_sink(gr.sizeof_char,outfile)

	# Connect it all together
	tb.connect( srcf, is2c, rrc, ilp, duc, c2f, fpll, lp_filter)
	tb.connect( lp_filter, iir )
	tb.connect( lp_filter, (remove_dc, 0) )
	tb.connect( iir, (remove_dc, 1) )
	tb.connect( remove_dc, btl )
	tb.connect( (btl, 0), (fsc, 0), (eq, 0), (fsd,0) )
	tb.connect( (btl, 1), (fsc, 1), (eq, 1), (fsd,1) )
	tb.connect( fsd, viterbi, deinter, rs_dec, derand, depad, outf )

	tb.run()

if __name__ == '__main__':
	graph (sys.argv[1:])

