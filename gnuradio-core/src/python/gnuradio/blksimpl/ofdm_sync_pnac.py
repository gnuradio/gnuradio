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

class ofdm_sync_pnac(gr.hier_block):
    def __init__(self, fg, fft_length, cp_length, ks):
        self.fg = fg

        # FIXME: when converting to hier_block2's, the output signature
        # should be the output of the divider (the normalized peaks) and
        # the angle value out of the sample and hold block
            
        self.input = gr.add_const_cc(0)

        symbol_length = fft_length + cp_length

        # PN Sync

        # autocorrelate with the known symbol
        ks = ks[0:fft_length//2]
        ks.reverse()
        self.crosscorr_filter = gr.fir_filter_ccc(1, ks)
        self.fg.connect(self.crosscorr_filter, gr.file_sink(gr.sizeof_gr_complex, "crosscorr.dat"))
        
        # Create a delay line
        self.delay = gr.delay(gr.sizeof_gr_complex, fft_length/2)

        # Correlation from ML Sync
        self.conjg = gr.conjugate_cc();
        self.corr = gr.multiply_cc();

        # Create a moving sum filter for the corr output
        moving_sum_taps = [1.0 for i in range(fft_length//2)]
        self.moving_sum_filter = gr.fir_filter_ccf(1,moving_sum_taps)

        # Create a moving sum filter for the input
        self.inputmag2 = gr.complex_to_mag_squared()
        movingsum2_taps = [1.0 for i in range(fft_length/2)]
        self.inputmovingsum = gr.fir_filter_fff(1,movingsum2_taps)
        self.square = gr.multiply_ff()
        self.normalize = gr.divide_ff()
     
        # Get magnitude (peaks) and angle (phase/freq error)
        self.c2mag = gr.complex_to_mag_squared()
        self.angle = gr.complex_to_arg()

        self.sample_and_hold = gr.sample_and_hold_ff()

        # Mix the signal with an NCO controlled by the sync loop
        nco_sensitivity = -1.0/fft_length
        self.nco = gr.frequency_modulator_fc(nco_sensitivity)
        self.sigmix = gr.multiply_cc()

        #ML measurements input to sampler block and detect
        self.sub1 = gr.add_const_ff(-1)
        self.pk_detect = gr.peak_detector_fb(0.2, 0.25, 30, 0.0005)

        self.sampler = gr.ofdm_sampler(fft_length,symbol_length)
        
        self.fg.connect(self.input, self.crosscorr_filter)
        self.fg.connect(self.crosscorr_filter, self.delay)
        self.fg.connect(self.crosscorr_filter, (self.corr,0))
        self.fg.connect(self.delay, self.conjg)
        self.fg.connect(self.conjg, (self.corr,1))
        self.fg.connect(self.corr, self.moving_sum_filter)
        self.fg.connect(self.moving_sum_filter, self.c2mag)
        self.fg.connect(self.moving_sum_filter, self.angle)
        self.fg.connect(self.angle, (self.sample_and_hold,0))
        self.fg.connect(self.sample_and_hold, self.nco)

        self.fg.connect(self.input, (self.sigmix,0))
        self.fg.connect(self.nco, (self.sigmix,1))
        self.fg.connect(self.sigmix, (self.sampler,0))

        self.fg.connect(self.input, self.inputmag2, self.inputmovingsum)
        self.fg.connect(self.inputmovingsum, (self.square,0))
        self.fg.connect(self.inputmovingsum, (self.square,1))
        self.fg.connect(self.square, (self.normalize,1))
        self.fg.connect(self.c2mag, (self.normalize,0))
        self.fg.connect(self.normalize, self.sub1, self.pk_detect)

        self.fg.connect(self.pk_detect, (self.sampler,1))
        self.fg.connect(self.pk_detect, (self.sample_and_hold,1))
            

        if 1:
            self.fg.connect(self.normalize, gr.file_sink(gr.sizeof_float,
                                                         "ofdm_sync_pnac-theta_f.dat"))
            self.fg.connect(self.angle, gr.file_sink(gr.sizeof_float,
                                                     "ofdm_sync_pnac-epsilon_f.dat"))
            self.fg.connect(self.pk_detect, gr.file_sink(gr.sizeof_char,
                                                         "ofdm_sync_pnac-peaks_b.dat"))
            self.fg.connect(self.sigmix, gr.file_sink(gr.sizeof_gr_complex,
                                                      "ofdm_sync_pnac-sigmix_c.dat"))
            self.fg.connect(self.sampler, gr.file_sink(gr.sizeof_gr_complex*fft_length,
                                                       "ofdm_sync_pnac-sampler_c.dat"))
            self.fg.connect(self.sample_and_hold, gr.file_sink(gr.sizeof_float,
                                                               "ofdm_sync_pnac-sample_and_hold_f.dat"))
            self.fg.connect(self.nco, gr.file_sink(gr.sizeof_gr_complex,
                                                   "ofdm_sync_pnac-nco_c.dat"))
            self.fg.connect(self.input, gr.file_sink(gr.sizeof_gr_complex,
                                                     "ofdm_sync_pnac-input_c.dat"))

        gr.hier_block.__init__(self, fg, self.input, self.sampler)
