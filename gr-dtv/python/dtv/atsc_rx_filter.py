#!/usr/bin/env /usr/bin/python
#
# Copyright 2014 Free Software Foundation, Inc.
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

from gnuradio import gr, filter
import dtv_swig as dtv

# FIXME move these into separate constants module
ATSC_CHANNEL_BW   = 6.0e6
ATSC_SYMBOL_RATE  = 4.5e6/286*684 # ~10.76 Mbaud
ATSC_RRC_SYMS     = 8             # filter kernel extends over 2N+1 symbols

class atsc_rx_filter(gr.hier_block2):
    def __init__(self, input_rate, sps):
        gr.hier_block2.__init__(self, "atsc_rx_filter",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        # Create matched RX filter with RRC response for fractional
        # interpolator.
        nfilts = 32
        output_rate = ATSC_SYMBOL_RATE*sps # Desired oversampled sample rate
        filter_rate = input_rate*nfilts
        symbol_rate = ATSC_SYMBOL_RATE/2.0 # One-sided bandwidth of sideband
        excess_bw = 0.1152 #1.0-(0.5*ATSC_SYMBOL_RATE/ATSC_CHANNEL_BW) # ~10.3%
        ntaps = int((2*ATSC_RRC_SYMS+1)*sps*nfilts)
        interp = output_rate/input_rate
        gain = nfilts*symbol_rate/filter_rate
        rrc_taps = filter.firdes.root_raised_cosine(gain,             # Filter gain
                                                    filter_rate,      # PFB filter prototype rate
                                                    symbol_rate,      # ATSC symbol rate
                                                    excess_bw,        # ATSC RRC excess bandwidth
                                                    ntaps)            # Length of filter

        pfb = filter.pfb_arb_resampler_ccf(interp, rrc_taps, nfilts)

        # Connect pipeline
        self.connect(self, pfb, self)
