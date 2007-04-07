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

from gnuradio import gr
from gnuradio.vocoder import cvsd_vocoder

class cvsd_encode(gr.hier_block):
    def __init__(self, fg, resample=8):
        scale_factor = 32000.0
        self.interp = resample

        src_scale = gr.multiply_const_ff(scale_factor)
        interp = gr.interp_fir_filter_fff(self.interp, self.interp*(1,))
        f2s = gr.float_to_short()
        enc = cvsd_vocoder.encode_sb()

        fg.connect(src_scale, interp, f2s, enc)
        gr.hier_block.__init__(self, fg, src_scale, enc)


class cvsd_decode(gr.hier_block):
    def __init__(self, fg, resample=8):
        scale_factor = 32000.0
        self.decim = resample

        dec = cvsd_vocoder.decode_bs()
        s2f = gr.short_to_float()
        decim = gr.fir_filter_fff(self.decim, (1,))
        sink_scale = gr.multiply_const_ff(1.0/scale_factor)

        fg.connect(dec, s2f, decim, sink_scale)
        gr.hier_block.__init__(self, fg, dec, sink_scale)

