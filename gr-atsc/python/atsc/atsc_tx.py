#!/usr/bin/env /usr/bin/python
#
# Copyright 2013 Free Software Foundation, Inc.
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
        	raise ValueError('usage: atsc_tx.py input_file.ts outfile\n')

	tb = gr.top_block()

	srcf = blocks.file_source(gr.sizeof_char, infile)

	pad = atsc.pad()
	rand = atsc.randomizer()
	rs_enc = atsc.rs_encoder()
	inter = atsc.interleaver()
	trellis = atsc.trellis_encoder()
	ds2sds = atsc.ds_to_softds()
	fsm = atsc.field_sync_mux()
	
	# Then we demod for testing
	fsd = atsc.field_sync_demux()
	viterbi = atsc.viterbi_decoder()
	deinter = atsc.deinterleaver()
	rs_dec = atsc.rs_decoder()
	derand = atsc.derandomizer()
	depad = atsc.depad()

	outf = blocks.file_sink(gr.sizeof_char, outfile)

	tb.connect( srcf, pad, rand, rs_enc, inter, trellis, ds2sds, viterbi, deinter, rs_dec, derand, depad, outf )

	tb.run()


if __name__ == '__main__':
	graph (sys.argv[1:])
