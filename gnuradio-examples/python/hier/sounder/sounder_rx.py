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
from usrp_source import usrp_source_c
import math

n2s = eng_notation.num_to_str

class sounder_rx(gr.hier_block2):
    """
    Creates a channel sounder receiver block with the given parameters.
    """

    def __init__(self, chip_rate, degree, verbose):

        # Call hierarchical block constructor
        # Top-level blocks have no inputs or outputs
        gr.hier_block2.__init__(self,
                                "sounder_rx",                                # Block typename
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._degree = degree
        self._chip_rate = chip_rate
        self._verbose = verbose
        self._length = 2**self._degree-1
        self._rep_rate = self._chip_rate/float(self._length)

        if self._verbose:
            print "Using PN sequence of degree", self._degree, "length", self._length
            print "Sequence repetition rate is", n2s(self._rep_rate), "per sec"
        
        self.connect(self, gr.pn_correlator_cc(self._degree), self)
