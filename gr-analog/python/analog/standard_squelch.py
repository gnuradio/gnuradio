#
# Copyright 2005,2007,2012 Free Software Foundation, Inc.
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
from gnuradio import blocks
from gnuradio import filter

class standard_squelch(gr.hier_block2):
    def __init__(self, audio_rate):
	gr.hier_block2.__init__(self, "standard_squelch",
				gr.io_signature(1, 1, gr.sizeof_float), # Input signature
				gr.io_signature(1, 1, gr.sizeof_float)) # Output signature

        self.input_node = blocks.add_const_ff(0)          # FIXME kludge

        self.low_iir = filter.iir_filter_ffd((0.0193,0,-0.0193),(1,1.9524,-0.9615))
        self.low_square = blocks.multiply_ff()
        self.low_smooth = filter.single_pole_iir_filter_ff(1/(0.01*audio_rate))   # 100ms time constant

        self.hi_iir = filter.iir_filter_ffd((0.0193,0,-0.0193),(1,1.3597,-0.9615))
        self.hi_square = blocks.multiply_ff()
        self.hi_smooth = filter.single_pole_iir_filter_ff(1/(0.01*audio_rate))

        self.sub = blocks.sub_ff();
        self.add = blocks.add_ff();
        self.gate = blocks.threshold_ff(0.3,0.43,0)
        self.squelch_lpf = filter.single_pole_iir_filter_ff(1/(0.01*audio_rate))

        self.div = blocks.divide_ff()
        self.squelch_mult = blocks.multiply_ff()

	self.connect(self, self.input_node)
        self.connect(self.input_node, (self.squelch_mult, 0))

        self.connect(self.input_node,self.low_iir)
        self.connect(self.low_iir,(self.low_square,0))
        self.connect(self.low_iir,(self.low_square,1))
        self.connect(self.low_square,self.low_smooth,(self.sub,0))
        self.connect(self.low_smooth, (self.add,0))

        self.connect(self.input_node,self.hi_iir)
        self.connect(self.hi_iir,(self.hi_square,0))
        self.connect(self.hi_iir,(self.hi_square,1))
        self.connect(self.hi_square,self.hi_smooth,(self.sub,1))
        self.connect(self.hi_smooth, (self.add,1))

        self.connect(self.sub, (self.div, 0))
        self.connect(self.add, (self.div, 1))
        self.connect(self.div, self.gate, self.squelch_lpf, (self.squelch_mult,1))
	self.connect(self.squelch_mult, self)

    def set_threshold(self, threshold):
        self.gate.set_hi(threshold)

    def threshold(self):
        return self.gate.hi()

    def squelch_range(self):
        return (0.0, 1.0, 1.0/100)
