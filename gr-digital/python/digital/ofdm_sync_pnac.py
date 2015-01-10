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

class ofdm_sync_pnac(gr.hier_block2):
    def __init__(self, fft_length, cp_length, kstime, logging=False):
        """
        OFDM synchronization using PN Correlation and initial cross-correlation:
        F. Tufvesson, O. Edfors, and M. Faulkner, "Time and Frequency Synchronization for OFDM using
        PN-Sequency Preambles," IEEE Proc. VTC, 1999, pp. 2203-2207.

        This implementation is meant to be a more robust version of the Schmidl and Cox receiver design.
        By correlating against the preamble and using that as the input to the time-delayed correlation,
        this circuit produces a very clean timing signal at the end of the preamble. The timing is
        more accurate and does not have the problem associated with determining the timing from the
        plateau structure in the Schmidl and Cox.

        This implementation appears to require that the signal is received with a normalized power or signal
        scaling factor to reduce ambiguities introduced from partial correlation of the cyclic prefix and
        the peak detection. A better peak detection block might fix this.

        Also, the cross-correlation falls apart as the frequency offset gets larger and completely fails
        when an integer offset is introduced. Another thing to look at.
        """

        gr.hier_block2.__init__(self, "ofdm_sync_pnac",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature2(2, 2, gr.sizeof_float, gr.sizeof_char)) # Output signature

        self.input = blocks.add_const_cc(0)

        symbol_length = fft_length + cp_length

        # PN Sync with cross-correlation input

        # cross-correlate with the known symbol
        kstime = [k.conjugate() for k in kstime[0:fft_length//2]]
        kstime.reverse()
        self.crosscorr_filter = filter.fir_filter_ccc(1, kstime)
        
        # Create a delay line
        self.delay = blocks.delay(gr.sizeof_gr_complex, fft_length/2)

        # Correlation from ML Sync
        self.conjg = blocks.conjugate_cc();
        self.corr = blocks.multiply_cc();

        # Create a moving sum filter for the input
        self.mag = blocks.complex_to_mag_squared()
        self.power = filter.fir_filter_fff(1, [1.0] * int(fft_length))

        # Get magnitude (peaks) and angle (phase/freq error)
        self.c2mag = blocks.complex_to_mag_squared()
        self.angle = blocks.complex_to_arg()
        self.compare = blocks.sub_ff()

        self.sample_and_hold = blocks.sample_and_hold_ff()

        #ML measurements input to sampler block and detect
        self.threshold = blocks.threshold_ff(0,0,0)      # threshold detection might need to be tweaked
        self.peaks = blocks.float_to_char()

        self.connect(self, self.input)

        # Cross-correlate input signal with known preamble
        self.connect(self.input, self.crosscorr_filter)

        # use the output of the cross-correlation as input time-shifted correlation
        self.connect(self.crosscorr_filter, self.delay)
        self.connect(self.crosscorr_filter, (self.corr,0))
        self.connect(self.delay, self.conjg)
        self.connect(self.conjg, (self.corr,1))
        self.connect(self.corr, self.c2mag)
        self.connect(self.corr, self.angle)
        self.connect(self.angle, (self.sample_and_hold,0))
        
        # Get the power of the input signal to compare against the correlation
        self.connect(self.crosscorr_filter, self.mag, self.power)

        # Compare the power to the correlator output to determine timing peak
        # When the peak occurs, it peaks above zero, so the thresholder detects this
        self.connect(self.c2mag, (self.compare,0))
        self.connect(self.power, (self.compare,1))
        self.connect(self.compare, self.threshold)
        self.connect(self.threshold, self.peaks, (self.sample_and_hold,1))

        # Set output signals
        #    Output 0: fine frequency correction value
        #    Output 1: timing signal
        self.connect(self.sample_and_hold, (self,0))
        self.connect(self.peaks, (self,1))

        if logging:
            self.connect(self.compare, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pnac-compare_f.dat"))
            self.connect(self.c2mag, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pnac-theta_f.dat"))
            self.connect(self.power, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pnac-inputpower_f.dat"))
            self.connect(self.angle, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pnac-epsilon_f.dat"))
            self.connect(self.threshold, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pnac-threshold_f.dat"))
            self.connect(self.peaks, blocks.file_sink(gr.sizeof_char, "ofdm_sync_pnac-peaks_b.dat"))
            self.connect(self.sample_and_hold, blocks.file_sink(gr.sizeof_float, "ofdm_sync_pnac-sample_and_hold_f.dat"))
            self.connect(self.input, blocks.file_sink(gr.sizeof_gr_complex, "ofdm_sync_pnac-input_c.dat"))
