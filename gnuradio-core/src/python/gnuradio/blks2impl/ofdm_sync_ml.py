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
from gnuradio import gr

class ofdm_sync_ml(gr.hier_block2):
    def __init__(self, fft_length, cp_length, snr, logging):
        ''' Maximum Likelihood OFDM synchronizer:
        J. van de Beek, M. Sandell, and P. O. Borjesson, "ML Estimation
        of Time and Frequency Offset in OFDM Systems," IEEE Trans.
        Signal Processing, vol. 45, no. 7, pp. 1800-1805, 1997.
        '''

        # FIXME: change the output signature
        # should be the output of the divider (the normalized peaks) and
        # the angle value out of the sample and hold block
        # move sampler out of this block

	gr.hier_block2.__init__(self, "ofdm_sync_ml",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex*fft_length)) # Output signature

        self.input = gr.add_const_cc(0)

        SNR = 10.0**(snr/10.0)
        rho = SNR / (SNR + 1.0)
        symbol_length = fft_length + cp_length

        # ML Sync

        # Energy Detection from ML Sync

        self.connect(self, self.input)

        # Create a delay line
        self.delay = gr.delay(gr.sizeof_gr_complex, fft_length)
        self.connect(self.input, self.delay)

        # magnitude squared blocks
        self.magsqrd1 = gr.complex_to_mag_squared()
        self.magsqrd2 = gr.complex_to_mag_squared()
        self.adder = gr.add_ff()

        moving_sum_taps = [rho/2 for i in range(cp_length)]
        self.moving_sum_filter = gr.fir_filter_fff(1,moving_sum_taps)
        
        self.connect(self.input,self.magsqrd1)
        self.connect(self.delay,self.magsqrd2)
        self.connect(self.magsqrd1,(self.adder,0))
        self.connect(self.magsqrd2,(self.adder,1))
        self.connect(self.adder,self.moving_sum_filter)
        

        # Correlation from ML Sync
        self.conjg = gr.conjugate_cc();
        self.mixer = gr.multiply_cc();

        movingsum2_taps = [1.0 for i in range(cp_length)]
        self.movingsum2 = gr.fir_filter_ccf(1,movingsum2_taps)
        
        # Correlator data handler
        self.c2mag = gr.complex_to_mag()
        self.angle = gr.complex_to_arg()
        self.connect(self.input,(self.mixer,1))
        self.connect(self.delay,self.conjg,(self.mixer,0))
        self.connect(self.mixer,self.movingsum2,self.c2mag)
        self.connect(self.movingsum2,self.angle)

        # ML Sync output arg, need to find maximum point of this
        self.diff = gr.sub_ff()
        self.connect(self.c2mag,(self.diff,0))
        self.connect(self.moving_sum_filter,(self.diff,1))

        #ML measurements input to sampler block and detect
        nco_sensitivity = -1.0/fft_length
        self.f2c = gr.float_to_complex()
        self.sampler = gr.ofdm_sampler(fft_length,symbol_length)
        self.pk_detect = gr.peak_detector_fb(0.2, 0.25, 30, 0.0005)
        #self.pk_detect = gr.peak_detector2_fb()
        self.sample_and_hold = gr.sample_and_hold_ff()
        self.nco = gr.frequency_modulator_fc(nco_sensitivity)
        self.sigmix = gr.multiply_cc()

        # Mix the signal with an NCO controlled by the sync loop
        self.connect(self.input, (self.sigmix,0))
        self.connect(self.nco, (self.sigmix,1))
        self.connect(self.sigmix, (self.sampler,0))

        # use the sync loop values to set the sampler and the NCO
        #     self.diff = theta
        #     self.angle = epsilon
                          
        self.connect(self.diff, self.pk_detect)

        use_dpll = 1
        if use_dpll:
            self.dpll = gr.dpll_bb(float(symbol_length),0.01)
            self.connect(self.pk_detect, self.dpll)
            self.connect(self.dpll, (self.sampler,1))
            self.connect(self.dpll, (self.sample_and_hold,1))
        else:
            self.connect(self.pk_detect, (self.sampler,1))
            self.connect(self.pk_detect, (self.sample_and_hold,1))
            
        self.connect(self.angle, (self.sample_and_hold,0))
        self.connect(self.sample_and_hold, self.nco)

        self.connect(self.sampler, self)

        if logging:
            self.connect(self.diff, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-theta_f.dat"))
            self.connect(self.angle, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-epsilon_f.dat"))
            self.connect(self.pk_detect, gr.file_sink(gr.sizeof_char, "ofdm_sync_ml-peaks_b.dat"))
            if use_dpll:
                self.connect(self.dpll, gr.file_sink(gr.sizeof_char, "ofdm_sync_ml-dpll_b.dat"))

            self.connect(self.sigmix, gr.file_sink(gr.sizeof_gr_complex, "ofdm_sync_ml-sigmix_c.dat"))
            self.connect(self.sampler, gr.file_sink(gr.sizeof_gr_complex*fft_length, "ofdm_sync_ml-sampler_c.dat"))
            self.connect(self.sample_and_hold, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-sample_and_hold_f.dat"))
            self.connect(self.nco, gr.file_sink(gr.sizeof_gr_complex, "ofdm_sync_ml-nco_c.dat"))
            self.connect(self.input, gr.file_sink(gr.sizeof_gr_complex, "ofdm_sync_ml-input_c.dat"))

