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

from gnuradio import gr, filter, analog
from .atsc_rx_filter import *

class atsc_rx(gr.hier_block2):
    def __init__(self, input_rate, sps):
        gr.hier_block2.__init__(self, "atsc_rx",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_char))       # Output signature

        # ATSC receiver filter/interpolator
        rx_filt = atsc_rx_filter(input_rate, sps)

        # Lock on to pilot tone, shift to DC, then discard Q channel
        output_rate = ATSC_SYMBOL_RATE*sps
        pll = dtv.atsc_fpll(output_rate)

        # Remove pilot tone now at DC
        dcr = filter.dc_blocker_ff(4096)

        # Normalize signal to proper constellation amplitude
        agc = analog.agc_ff(1e-5, 4.0)

        # Synchronize bit and segment timing
        btl = dtv.atsc_sync(output_rate)

        # Check for correct field sync
        fsc = dtv.atsc_fs_checker()

        # Equalize channel using training sequences
        equ = dtv.atsc_equalizer()

        # Remove convolutional trellis coding
        vit = dtv.atsc_viterbi_decoder()

        # Remove convolutional interleaving
        dei = dtv.atsc_deinterleaver()

        # Reed-Solomon decode
        rsd = dtv.atsc_rs_decoder()

        # Derandomize MPEG2-TS packet
        der = dtv.atsc_derandomizer()

        # Remove padding from packet
        dep = dtv.atsc_depad()

        # Connect pipeline
        self.connect(self, rx_filt, pll, dcr, agc, btl, fsc)
        self.connect((fsc,0),(equ,0))
        self.connect((fsc,1),(equ,1))
        self.connect((equ,0),(vit,0))
        self.connect((equ,1),(vit,1))
        self.connect((vit,0),(dei,0))
        self.connect((vit,1),(dei,1))
        self.connect((dei,0),(rsd,0))
        self.connect((dei,1),(rsd,1))
        self.connect((rsd,0),(der,0))
        self.connect((rsd,1),(der,1))
        self.connect((der,0),(dep,0))
        self.connect((dep,0),(self,0))
