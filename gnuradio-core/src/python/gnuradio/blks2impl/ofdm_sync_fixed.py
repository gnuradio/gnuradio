#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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

import math
from gnuradio import gr

class ofdm_sync_fixed(gr.hier_block2):
    def __init__(self, fft_length, cp_length, nsymbols, freq_offset, logging=False):

        gr.hier_block2.__init__(self, "ofdm_sync_fixed",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature2(2, 2, gr.sizeof_float, gr.sizeof_char)) # Output signature

        # Use a fixed trigger point instead of sync block
        symbol_length = fft_length + cp_length
        pkt_length = nsymbols*symbol_length
        data = (pkt_length)*[0,]
        data[(symbol_length)-1] = 1
        self.peak_trigger = gr.vector_source_b(data, True)

        # Use a pre-defined frequency offset
        foffset = (pkt_length)*[math.pi*freq_offset,]
        self.frequency_offset = gr.vector_source_f(foffset, True)

        self.connect(self, gr.null_sink(gr.sizeof_gr_complex))
        self.connect(self.frequency_offset, (self,0))
        self.connect(self.peak_trigger, (self,1))

        if logging:
            self.connect(self.peak_trigger, gr.file_sink(gr.sizeof_char, "ofdm_sync_fixed-peaks_b.dat"))

