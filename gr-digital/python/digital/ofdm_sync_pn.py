#!/usr/bin/env python
#
# Copyright 2007,2008 Free Software Foundation, Inc.
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
from numpy import fft
from gnuradio import gr

try:
    from gnuradio import filter
except ImportError:
    import filter_swig as filter

try:
    from gnuradio import blocks
except ImportError:
    import blocks_swig as blocks

class ofdm_sync_pn(gr.hier_block2):
    def __init__(self, fft_length, cp_length, logging=False):
        """
        OFDM synchronization using PN Correlation:
        T. M. Schmidl and D. C. Cox, "Robust Frequency and Timing
        Synchronization for OFDM," IEEE Trans. Communications, vol. 45,
        no. 12, 1997.
        """

        gr.hier_block2.__init__(self, "ofdm_sync_pn",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature2(2, 2, gr.sizeof_float, gr.sizeof_char)) # Output signature

        self.input = blocks.add_const_cc(0)

        # PN Sync

        # Create a delay line
        self.delay = blocks.delay(gr.sizeof_gr_complex, fft_length/2)

        # Correlation from ML Sync
        self.conjg = blocks.conjugate_cc();
        self.corr = blocks.multiply_cc();

        # Create a moving sum filter for the corr output
        self.moving_sum_filter = filter.fir_filter_ccf(1, [1.0] * (fft_length//2))

        # Create a moving sum filter for the input
        self.inputmag2 = blocks.complex_to_mag_squared()
        self.inputmovingsum = filter.fir_filter_fff(1, [1.0] * (fft_length//2))

        self.square = blocks.multiply_ff()
        self.normalize = blocks.divide_ff()

        # Get magnitude (peaks) and angle (phase/freq error)
        self.c2mag = blocks.complex_to_mag_squared()
        self.angle = blocks.complex_to_arg()

        self.sample_and_hold = blocks.sample_and_hold_ff()

        #ML measurements input to sampler block and detect
        self.sub1 = blocks.add_const_ff(-1)
        self.pk_detect = blocks.peak_detector_fb(0.20, 0.20, 30, 0.001)

        self.connect(self, self.input)

        # Calculate the frequency offset from the correlation of the preamble
        self.connect(self.input, self.delay)
        self.connect(self.input, (self.corr,0))
        self.connect(self.delay, self.conjg)
        self.connect(self.conjg, (self.corr,1))
        self.connect(self.corr, self.moving_sum_filter)
        self.connect(self.moving_sum_filter, self.c2mag)
        self.connect(self.moving_sum_filter, self.angle)
        self.connect(self.angle, (self.sample_and_hold,0))

        # Get the power of the input signal to normalize the output of the correlation
        self.connect(self.input, self.inputmag2, self.inputmovingsum)
        self.connect(self.inputmovingsum, (self.square,0))
        self.connect(self.inputmovingsum, (self.square,1))
        self.connect(self.square, (self.normalize,1))
        self.connect(self.c2mag, (self.normalize,0))

        # Create a moving sum filter for the corr output
        matched_filter_taps = [1.0/cp_length for i in range(cp_length)]
        self.matched_filter = filter.fir_filter_fff(1,matched_filter_taps)
        self.connect(self.normalize, self.matched_filter)
        
        self.connect(self.matched_filter, self.sub1, self.pk_detect)
        #self.connect(self.matched_filter, self.pk_detect)
        self.connect(self.pk_detect, (self.sample_and_hold,1))

        # Set output signals
        #    Output 0: fine frequency correction value
        #    Output 1: timing signal
        self.connect(self.sample_and_hold, (self,0))
        self.connect(self.pk_detect, (self,1))

        if logging:
            self.connect(self.matched_filter, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pn-mf_f.dat"))
            self.connect(self.normalize, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pn-theta_f.dat"))
            self.connect(self.angle, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pn-epsilon_f.dat"))
            self.connect(self.pk_detect, blocks.file_sink(gr.sizeof_char, "ofdm_sync_pn-peaks_b.dat"))
            self.connect(self.sample_and_hold, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pn-sample_and_hold_f.dat"))
            self.connect(self.input, blocks.file_sink(gr.sizeof_gr_complex, "ofdm_sync_pn-input_c.dat"))
