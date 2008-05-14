#!/usr/bin/env python
#
# Copyright 2004,2005 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr, atsc
import math, os

def main():

	print os.getpid()

	tb = gr.top_block()

        u = gr.file_source(gr.sizeof_float,"/tmp/atsc_pipe_2")

        input_rate = 19.2e6
	IF_freq = 5.75e6


	# 1/2 as wide because we're designing lp filter
	symbol_rate = atsc.ATSC_SYMBOL_RATE/2. 
	NTAPS = 279
	tt = gr.firdes.root_raised_cosine (1.0, input_rate, symbol_rate, .115, NTAPS)
  # heterodyne the low pass coefficients up to the specified bandpass
  # center frequency.  Note that when we do this, the filter bandwidth
  # is effectively twice the low pass (2.69 * 2 = 5.38) and hence
  # matches the diagram in the ATSC spec.
	arg = 2. * math.pi * IF_freq / input_rate
	t=[]
	for i in range(len(tt)):
	  t += [tt[i] * 2. * math.cos(arg * i)]
	rrc = gr.fir_filter_fff(1, t)

	fpll = atsc.fpll()

	pilot_freq = IF_freq - 3e6 + 0.31e6
	lower_edge = 6e6 - 0.31e6
	upper_edge = IF_freq - 3e6 + pilot_freq
	transition_width = upper_edge - lower_edge
	lp_coeffs = gr.firdes.low_pass (1.0,
			   input_rate,
			   (lower_edge + upper_edge) * 0.5,
                           transition_width,
                           gr.firdes.WIN_HAMMING);

	lp_filter = gr.fir_filter_fff (1,lp_coeffs)

	alpha = 1e-5
	iir = gr.single_pole_iir_filter_ff(alpha)
	remove_dc = gr.sub_ff()

	out = gr.file_sink(gr.sizeof_float,"/tmp/atsc_pipe_3")
	# out = gr.file_sink(gr.sizeof_float,"/mnt/sata/atsc_data_float")

        tb.connect(u, fpll, lp_filter)
	tb.connect(lp_filter, iir)
	tb.connect(lp_filter, (remove_dc,0))
	tb.connect(iir, (remove_dc,1))
	tb.connect(remove_dc, out)

	tb.run()


if __name__ == '__main__':
    main ()



