#!/usr/bin/env python
#
# Copyright 2008 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr
from math import pi, log10
import cmath

class transmit_path(gr.hier_block2):
    """
    This transmits a BERT sequence of bits using filtered BPSK and
    outputs the complex baseband waveform.
    """
    def __init__(self,
                 sps,          # Samples per symbol
                 excess_bw,    # RRC filter excess bandwidth (typically 0.35-0.5)
                 amplitude     # DAC output level, 0-32767, typically 2000-8000
                 ):

        gr.hier_block2.__init__(self, "transmit_path",
                                gr.io_signature(0, 0, 0),                    # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        # Create BERT data bit stream	
	self._bits = gr.vector_source_b([1,], True)      # Infinite stream of ones
        self._scrambler = gr.scrambler_bb(0x8A, 0x7F, 7) # CCSDS 7-bit scrambler

	# Map to constellation
	self._constellation = [-1+0j, 1+0j]
	self._mapper = gr.chunks_to_symbols_bc(self._constellation)	

	# Create RRC with specified excess bandwidth
	taps = gr.firdes.root_raised_cosine(sps*amplitude,  # Gain
					    sps,	    # Sampling rate
					    1.0,	    # Symbol rate
					    excess_bw,	    # Roll-off factor
					    11*sps)         # Number of taps

	self._rrc = gr.interp_fir_filter_ccf(sps,	    # Interpolation rate
					     taps)	    # FIR taps

        # Wire block inputs and outputs
        self.connect(self._bits, self._scrambler, self._mapper, self._rrc, self)

