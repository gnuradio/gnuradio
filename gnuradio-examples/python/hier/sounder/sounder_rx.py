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
    Creates a top-level channel sounder block with the given parameters.
    """

    def __init__(self, subdev_spec, freq, cal, verbose, max_delay, chip_rate, gain):

        # Call hierarchical block constructor
        # Top-level blocks have no inputs or outputs
        gr.hier_block2.__init__(self,
                                "sounder_rx",           # Block typename
                                gr.io_signature(0,0,0), # Input signature
                                gr.io_signature(0,0,0)) # Output signature
        self._freq = freq
        self._cal = cal
        self._verbose = verbose
        self._max_delay = max_delay

        self._u = usrp_source_c(0, subdev_spec, gain, chip_rate, self._freq, self._cal, verbose)
        self.define_component("usrp", self._u)

        self._chip_rate = self._u._if_rate
        self._resolution = 1.0/self._chip_rate

        min_chips = int(math.ceil(2.0*self._max_delay * self._chip_rate))
        degree = int(math.ceil(math.log(min_chips)/math.log(2)))
        self._length = 2**degree-1
        self._seq_per_sec = self._chip_rate/self._length
        self._tap = 0.0001

        if self._verbose:
            print "Actual chip rate is", n2s(self._chip_rate), "chips/sec"
            print "Resolution is", n2s(self._resolution), "sec"
            print "Using specified maximum delay spread of", self._max_delay, "sec"
            print "Mininum sequence length needed is", n2s(min_chips), "chips"
            print "Using PN sequence of degree", degree, "length", self._length
            print "Sequences per second is", self._seq_per_sec
            print "IIR tap is", self._tap
        
        self.define_component("s2v", gr.stream_to_vector(gr.sizeof_gr_complex, self._length))
        self.define_component("fft", gr.fft_vcc(self._length, True, ())) # No window needed
        self.define_component("avg", gr.single_pole_iir_filter_cc(self._tap, self._length))
        self.define_component("keep", gr.keep_one_in_n(gr.sizeof_gr_complex*self._length, int(self._seq_per_sec)))
        self.define_component("sink", gr.file_sink(gr.sizeof_gr_complex*self._length, "FFT.dat"))

        self.connect("usrp", 0, "s2v", 0)
        self.connect("s2v", 0, "fft", 0)
        self.connect("fft", 0, "avg", 0)
        self.connect("avg", 0, "keep", 0)
        self.connect("keep", 0, "sink", 0)
