#!/usr/bin/env /usr/bin/python
#
# Copyright 2004 Free Software Foundation, Inc.
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
# All this module does is multiply the sample rate by 3, from 6.4e6 to
# 19.2e6 complex samples / sec, then lowpass filter with a cutoff of 3.2MHz
# and a transition band width of .5MHz.  Center of the tv channels is
# then at 0 with edges at -3.2MHz and 3.2MHz.

from gnuradio import gr
import sys, os

def graph (args):

    print os.getpid()

    nargs = len (args)
    if nargs == 1:
	infile = args[0]
    else:
	sys.stderr.write('usage: interp.py input_file\n')
	sys.exit (1)

    tb = gr.top_block ()

    srcf = gr.file_source (gr.sizeof_short,infile)
    s2ss = gr.stream_to_streams(gr.sizeof_short,2)
    s2f1 = gr.short_to_float()
    s2f2 = gr.short_to_float()
    src0 = gr.float_to_complex()


    lp_coeffs = gr.firdes.low_pass ( 3, 19.2e6, 3.2e6, .5e6, gr.firdes.WIN_HAMMING )
    lp = gr.interp_fir_filter_ccf ( 3, lp_coeffs )

    file = gr.file_sink(gr.sizeof_gr_complex,"/tmp/atsc_pipe_1")

    tb.connect( srcf, s2ss )
    tb.connect( (s2ss, 0), s2f1, (src0,0) )
    tb.connect( (s2ss, 1), s2f2, (src0,1) )
    tb.connect( src0, lp, file)

    tb.start()
    raw_input ('Head End: Press Enter to stop')
    tb.stop()

if __name__ == '__main__':
    graph (sys.argv[1:])


