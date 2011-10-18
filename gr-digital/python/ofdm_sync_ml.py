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
from gnuradio import gr

class ofdm_sync_ml(gr.hier_block2):
    def __init__(self, fft_length, cp_length, snr, kstime, logging):
        ''' Maximum Likelihood OFDM synchronizer:
        J. van de Beek, M. Sandell, and P. O. Borjesson, "ML Estimation
        of Time and Frequency Offset in OFDM Systems," IEEE Trans.
        Signal Processing, vol. 45, no. 7, pp. 1800-1805, 1997.
        '''

	gr.hier_block2.__init__(self, "ofdm_sync_ml",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature2(2, 2, gr.sizeof_float, gr.sizeof_char)) # Output signature

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
        self.f2c = gr.float_to_complex()
        self.pk_detect = gr.peak_detector_fb(0.2, 0.25, 30, 0.0005)
        self.sample_and_hold = gr.sample_and_hold_ff()

        # use the sync loop values to set the sampler and the NCO
        #     self.diff = theta
        #     self.angle = epsilon
                          
        self.connect(self.diff, self.pk_detect)

        # The DPLL corrects for timing differences between CP correlations
        use_dpll = 0
        if use_dpll:
            self.dpll = gr.dpll_bb(float(symbol_length),0.01)
            self.connect(self.pk_detect, self.dpll)
            self.connect(self.dpll, (self.sample_and_hold,1))
        else:
            self.connect(self.pk_detect, (self.sample_and_hold,1))
            
        self.connect(self.angle, (self.sample_and_hold,0))

        ################################
        # correlate against known symbol
        # This gives us the same timing signal as the PN sync block only on the preamble
        # we don't use the signal generated from the CP correlation because we don't want
        # to readjust the timing in the middle of the packet or we ruin the equalizer settings.
        kstime = [k.conjugate() for k in kstime]
        kstime.reverse()
        self.kscorr = gr.fir_filter_ccc(1, kstime)
        self.corrmag = gr.complex_to_mag_squared()
        self.div = gr.divide_ff()

        # The output signature of the correlation has a few spikes because the rest of the
        # system uses the repeated preamble symbol. It needs to work that generically if 
        # anyone wants to use this against a WiMAX-like signal since it, too, repeats.
        # The output theta of the correlator above is multiplied with this correlation to
        # identify the proper peak and remove other products in this cross-correlation
        self.threshold_factor = 0.1
        self.slice = gr.threshold_ff(self.threshold_factor, self.threshold_factor, 0)
        self.f2b = gr.float_to_char()
        self.b2f = gr.char_to_float()
        self.mul = gr.multiply_ff()
        
        # Normalize the power of the corr output by the energy. This is not really needed
        # and could be removed for performance, but it makes for a cleaner signal.
        # if this is removed, the threshold value needs adjustment.
        self.connect(self.input, self.kscorr, self.corrmag, (self.div,0))
        self.connect(self.moving_sum_filter, (self.div,1))
        
        self.connect(self.div, (self.mul,0))
        self.connect(self.pk_detect, self.b2f, (self.mul,1))
        self.connect(self.mul, self.slice)
        
        # Set output signals
        #    Output 0: fine frequency correction value
        #    Output 1: timing signal
        self.connect(self.sample_and_hold, (self,0))
        self.connect(self.slice, self.f2b, (self,1))


        if logging:
            self.connect(self.moving_sum_filter, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-energy_f.dat"))
            self.connect(self.diff, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-theta_f.dat"))
            self.connect(self.angle, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-epsilon_f.dat"))
            self.connect(self.corrmag, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-corrmag_f.dat"))
            self.connect(self.kscorr, gr.file_sink(gr.sizeof_gr_complex, "ofdm_sync_ml-kscorr_c.dat"))
            self.connect(self.div, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-div_f.dat"))
            self.connect(self.mul, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-mul_f.dat"))
            self.connect(self.slice, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-slice_f.dat"))
            self.connect(self.pk_detect, gr.file_sink(gr.sizeof_char, "ofdm_sync_ml-peaks_b.dat"))
            if use_dpll:
                self.connect(self.dpll, gr.file_sink(gr.sizeof_char, "ofdm_sync_ml-dpll_b.dat"))

            self.connect(self.sample_and_hold, gr.file_sink(gr.sizeof_float, "ofdm_sync_ml-sample_and_hold_f.dat"))
            self.connect(self.input, gr.file_sink(gr.sizeof_gr_complex, "ofdm_sync_ml-input_c.dat"))

