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
# -3MHz + 309KHz. Next a root raised cosine filter is aplied to match the one
# in the transmitter and thus reduce ISI. The phased locked loop then locks to
# the pilot and outputs just the real part of the signal ( as information is
# not stored in the phase with atsc ), this is then feed to the bit lock
# loop, this looks for the bit sync marker put at the beginning of every segment
# field, this then adjusts the timing so the amplitude will be sampled at the
# correct sample ( sub-sample is used in this case ). 
#
# Output is an MPEG-TS.

from gnuradio import gr, analog, atsc
from gnuradio import blocks
from gnuradio import filter
import sys, math, os, time

def graph (args):

	print os.getpid()

	nargs = len(args)
    	if nargs == 2:
		infile = args[0]
        	outfile = args[1]
    	else:
        	raise ValueError('usage: atsc_rx.py input_file output_file.ts\n')

	input_rate = 6.4e6*3

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
	ilp_coeffs = filter.firdes.low_pass(3, input_rate, 3.2e6, .5e6, filter.firdes.WIN_HAMMING)
	ilp = filter.interp_fir_filter_ccf(3, ilp_coeffs)
	
	# Phase locked loop
	fpll = atsc.fpll( input_rate )

	# fpll output is complex, we need floats
	c2f = blocks.complex_to_float()

	# Automatic gain control
	agc = analog.agc_ff(1e-6, 4)

	# Remove pilot ( at DC now )
	iir = filter.single_pole_iir_filter_ff(1e-5)
	remove_dc = blocks.sub_ff()

	# Bit Timing Loop, Field Sync Checker and Equalizer
	btl = atsc.bit_timing_loop( input_rate )
	fsc = atsc.fs_checker()
	eq = atsc.equalizer()

	# Viterbi, Deinterleaver, Reed_solomon, Derandomizer, and Depad
	viterbi = atsc.viterbi_decoder()
        deinter = atsc.deinterleaver()
        rs_dec = atsc.rs_decoder()
        derand = atsc.derandomizer()
	depad = atsc.depad()

	# Write to output file
	outf = blocks.file_sink(gr.sizeof_char,outfile)

	# Connect it all together
	tb.connect( srcf, is2c, rrc, ilp, fpll, c2f, agc)
	tb.connect( agc, (remove_dc, 0) )
	tb.connect( agc, iir, (remove_dc, 1) )
	tb.connect( remove_dc, btl, fsc, eq, viterbi, deinter, rs_dec, derand, depad, outf)

	tb.run()

	print 'srcf:      ' + repr(srcf.pc_work_time()).rjust(15) + ' / ' + repr(srcf.pc_nproduced()).rjust(8)
	print 'is2c:      ' + repr(is2c.pc_work_time()).rjust(15) + ' / ' + repr(is2c.pc_nproduced()).rjust(8)
	print 'rrc:       ' + repr(rrc.pc_work_time()).rjust(15) + ' / ' + repr(rrc.pc_nproduced()).rjust(8)
	print 'ilp:       ' + repr(ilp.pc_work_time()).rjust(15) + ' / ' + repr(ilp.pc_nproduced()).rjust(8)
	print 'fpll:      ' + repr(fpll.pc_work_time()).rjust(15) + ' / ' + repr(fpll.pc_nproduced()).rjust(8)
	print 'c2f:       ' + repr(c2f.pc_work_time()).rjust(15) + ' / ' + repr(c2f.pc_nproduced()).rjust(8)
	print 'agc:       ' + repr(agc.pc_work_time()).rjust(15) + ' / ' + repr(agc.pc_nproduced()).rjust(8)
	print 'btl:       ' + repr(btl.pc_work_time()).rjust(15) + ' / ' + repr(btl.pc_nproduced()).rjust(8)
	print 'fsc:       ' + repr(fsc.pc_work_time()).rjust(15) + ' / ' + repr(fsc.pc_nproduced()).rjust(8)
	print 'eq:        ' + repr(eq.pc_work_time()).rjust(15) + ' / ' + repr(eq.pc_nproduced()).rjust(8)
	print 'viterbi:   ' + repr(viterbi.pc_work_time()).rjust(15) + ' / ' + repr(viterbi.pc_nproduced()).rjust(8)
	print 'deinter:   ' + repr(deinter.pc_work_time()).rjust(15) + ' / ' + repr(deinter.pc_nproduced()).rjust(8)
	print 'rs_dec:    ' + repr(rs_dec.pc_work_time()).rjust(15) + ' / ' + repr(rs_dec.pc_nproduced()).rjust(8)
	print 'derand:    ' + repr(derand.pc_work_time()).rjust(15) + ' / ' + repr(derand.pc_nproduced()).rjust(8)
	print 'depad:     ' + repr(depad.pc_work_time()).rjust(15) + ' / ' + repr(depad.pc_nproduced()).rjust(8)


if __name__ == '__main__':
	graph (sys.argv[1:])

