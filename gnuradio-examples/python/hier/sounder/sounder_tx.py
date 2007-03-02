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

from gnuradio import gr, usrp, eng_notation
from usrp_sink import usrp_sink_c
import math

n2s = eng_notation.num_to_str

class sounder_tx(gr.hier_block2):
    """
    Creates a top-level channel sounder block with the given parameters.
    """

    def __init__(self, subdev_spec, freq, cal, verbose, max_delay, chip_rate, amplitude):

        # Call hierarchical block constructor
        # Top-level blocks have no inputs or outputs
        gr.hier_block2.__init__(self,
                                "sounder_tx",           # Block typename
                                gr.io_signature(0,0,0), # Input signature
                                gr.io_signature(0,0,0)) # Output signature
        self._freq = freq
        self._cal = cal
        self._verbose = verbose
        self._max_delay = max_delay

        self._u = usrp_sink_c(0, subdev_spec, chip_rate, self._freq, self._cal, verbose)
        self._chip_rate = self._u._if_rate
        self._resolution = 1.0/self._chip_rate

        min_chips = int(math.ceil(2.0*self._max_delay * self._chip_rate))
        degree = int(math.ceil(math.log(min_chips)/math.log(2)))
        self._length = 2**degree-1

        self._glfsr = gr.glfsr_source_b(degree)
        self._mapper = gr.chunks_to_symbols_bc((-amplitude+0j, amplitude+0j), 1)
        
        if self._verbose:
            print "Actual chip rate is", n2s(self._chip_rate), "chips/sec"
            print "Resolution is", n2s(self._resolution), "sec"
            print "Using specified maximum delay spread of", self._max_delay, "sec"
            print "Mininum sequence length needed is", n2s(min_chips), "chips"
            print "Using PN sequence of degree", degree, "length", self._length
            print "Output amplitude is", amplitude
            
        self.define_component("glfsr", self._glfsr)
        self.define_component("mapper", self._mapper)
        self.define_component("usrp", self._u)
        self.connect("glfsr", 0, "mapper", 0)
        self.connect("mapper", 0, "usrp", 0)
