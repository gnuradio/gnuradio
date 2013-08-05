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
from gnuradio.filter import firdes
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
	fsm = atsc.field_sync_mux()
	ds2sds = atsc.ds_to_softds()
	pilot = atsc.pilot()

	iff = filter.interp_fir_filter_fff(2, (firdes.low_pass(2, (4.5e6 / 286 * 684)*2, 3.2e6 * 3, 1e6, firdes.WIN_HAMMING)))
	f2c = blocks.float_to_complex(1)
        fxff = filter.freq_xlating_fir_filter_ccf(1, (firdes.root_raised_cosine (1.0, (4.5e6 / 286 * 684)*2, (4.5e6 / 286 * 684)/2, .1152, 279)), 3e6 - 0.309e6, (4.5e6 / 286 * 684)*2)
        rr = filter.rational_resampler_ccc(
                interpolation=5000,
                decimation=16815,
                taps=(firdes.low_pass(5000, 1, 0.8/5000, 0.3/5000, firdes.WIN_KAISER, 5.0)),
                fractional_bw=None,
        )
	c2is = blocks.complex_to_interleaved_short()

	outf = blocks.file_sink(gr.sizeof_short, outfile)

	tb.connect( srcf, pad, rand, rs_enc, inter, trellis, fsm, ds2sds, pilot, iff, f2c, fxff, rr, c2is, outf )

	tb.run()


if __name__ == '__main__':
	graph (sys.argv[1:])
