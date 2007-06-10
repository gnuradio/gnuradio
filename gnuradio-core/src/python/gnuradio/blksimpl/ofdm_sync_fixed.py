#!/usr/bin/env python
#
# Copyright 2007 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import math
from gnuradio import gr

class ofdm_sync_fixed(gr.hier_block):
    def __init__(self, fg, fft_length, cp_length, snr):
        self.fg = fg

        # Use a fixed trigger point instead of sync block
        data = (fft_length+cp_len)*[0,]
        data[(fft_length+cp_len)-1] = 1
        peak_trigger = gr.vector_source_b(data, True)
        
        self.fg.connect(peak_trigger, (self.sampler,1))

        if 1:
            self.fg.connect(self.sampler, gr.file_sink(gr.sizeof_gr_complex*fft_length,
                                                       "ofdm_sync_fixed-sampler_c.dat"))
        
        gr.hier_block.__init__(self, fg, (self.sampler,0), self.sampler)
