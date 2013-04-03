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

from gnuradio import gr, atsc
from gnuradio import filter
from gnuradio import blocks
import sys, os, math

def graph (args):

    nargs = len(args)
    if nargs == 2:
	infile = args[0]
        outfile = args[1]
    else:
        raise ValueError('usage: interp.py input_file output_file\n')

    tb = gr.top_block ()

    # Convert to a from shorts to a stream of complex numbers.
    srcf = blocks.file_source (gr.sizeof_short,infile)
    s2ss = blocks.stream_to_streams(gr.sizeof_short,2)
    s2f1 = blocks.short_to_float()
    s2f2 = blocks.short_to_float()
    src0 = blocks.float_to_complex()
    tb.connect(srcf, s2ss)
    tb.connect((s2ss, 0), s2f1, (src0, 0))
    tb.connect((s2ss, 1), s2f2, (src0, 1))

    # Low pass filter it and increase sample rate by a factor of 3.
    lp_coeffs = filter.firdes.low_pass ( 3, 19.2e6, 3.2e6, .5e6, filter.firdes.WIN_HAMMING )
    lp = filter.interp_fir_filter_ccf ( 3, lp_coeffs )
    tb.connect(src0, lp)

    # Upconvert it.
    duc_coeffs = filter.firdes.low_pass ( 1, 19.2e6, 9e6, 1e6, filter.firdes.WIN_HAMMING )
    duc = filter.freq_xlating_fir_filter_ccf ( 1, duc_coeffs, 5.75e6, 19.2e6 )
    # Discard the imaginary component.
    c2f = blocks.complex_to_float()
    tb.connect(lp, duc, c2f)

    # Frequency Phase Lock Loop
    input_rate = 19.2e6
    IF_freq = 5.75e6
    # 1/2 as wide because we're designing lp filter
    symbol_rate = atsc.ATSC_SYMBOL_RATE/2.
    NTAPS = 279
    tt = filter.firdes.root_raised_cosine (1.0, input_rate, symbol_rate, .115, NTAPS)
    # heterodyne the low pass coefficients up to the specified bandpass
    # center frequency.  Note that when we do this, the filter bandwidth
    # is effectively twice the low pass (2.69 * 2 = 5.38) and hence
    # matches the diagram in the ATSC spec.
    arg = 2. * math.pi * IF_freq / input_rate
    t=[]
    for i in range(len(tt)):
        t += [tt[i] * 2. * math.cos(arg * i)]
    rrc = filter.fir_filter_fff(1, t)

    fpll = atsc.fpll()

    pilot_freq = IF_freq - 3e6 + 0.31e6
    lower_edge = 6e6 - 0.31e6
    upper_edge = IF_freq - 3e6 + pilot_freq
    transition_width = upper_edge - lower_edge
    lp_coeffs = filter.firdes.low_pass(1.0,
                                       input_rate,
                                       (lower_edge + upper_edge) * 0.5,
                                       transition_width,
                                       filter.firdes.WIN_HAMMING);
    
    lp_filter = filter.fir_filter_fff(1,lp_coeffs)
    
    alpha = 1e-5
    iir = filter.single_pole_iir_filter_ff(alpha)
    remove_dc = blocks.sub_ff()

    tb.connect(c2f, fpll, lp_filter)
    tb.connect(lp_filter, iir)
    tb.connect(lp_filter, (remove_dc,0))
    tb.connect(iir, (remove_dc,1))
    
    # Bit Timing Loop, Field Sync Checker and Equalizer

    btl = atsc.bit_timing_loop()
    fsc = atsc.fs_checker()
    eq = atsc.equalizer()
    fsd = atsc.field_sync_demux()

    tb.connect(remove_dc, btl)
    tb.connect((btl, 0),(fsc, 0),(eq, 0),(fsd, 0))
    tb.connect((btl, 1),(fsc, 1),(eq, 1),(fsd, 1))

    # Viterbi

    viterbi = atsc.viterbi_decoder()
    deinter = atsc.deinterleaver()
    rs_dec = atsc.rs_decoder()
    derand = atsc.derandomizer()
    depad = atsc.depad()
    dst = blocks.file_sink(gr.sizeof_char, outfile)
    tb.connect(fsd, viterbi, deinter, rs_dec, derand, depad, dst)

    dst2 = blocks.file_sink(gr.sizeof_gr_complex, "atsc_complex.data")
    tb.connect(src0, dst2)

    tb.run ()

if __name__ == '__main__':
    graph (sys.argv[1:])


