#
# Copyright 2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, optfir, blks2
from math import pi
import pager_swig

class flex_demod(gr.hier_block2):
    """
    FLEX pager protocol demodulation block.

    This block demodulates a band-limited, complex down-converted baseband 
    channel into FLEX protocol frames.

    """

    def __init__(self, queue, freq=0.0, verbose=False, log=False):
	gr.hier_block2.__init__(self, "flex_demod",
				gr.io_signature(1, 1, gr.sizeof_gr_complex),
				gr.io_signature(0,0,0))

        k = 25000/(2*pi*1600)        # 4800 Hz max deviation
        quad = gr.quadrature_demod_cf(k)
	self.connect(self, quad)
	
        rsamp = blks2.rational_resampler_fff(16, 25)
        slicer = pager_swig.slicer_fb(5e-5) # DC removal averaging filter constant
	sync = pager_swig.flex_sync(16000)

        self.connect(quad, rsamp, slicer, sync)

	for i in range(4):
	    self.connect((sync, i), pager_swig.flex_deinterleave(), pager_swig.flex_parse(queue, freq))

	if log:
	    suffix = '_'+ "%3.3f" % (freq/1e6,) + '.dat'
	    quad_sink = gr.file_sink(gr.sizeof_float, 'quad'+suffix)
	    rsamp_sink = gr.file_sink(gr.sizeof_float, 'rsamp'+suffix)
	    slicer_sink = gr.file_sink(gr.sizeof_char, 'slicer'+suffix)
	    self.connect(rsamp, rsamp_sink)
	    self.connect(quad, quad_sink)
	    self.connect(slicer, slicer_sink)
	    