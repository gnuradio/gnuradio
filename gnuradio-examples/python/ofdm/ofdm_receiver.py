#!/usr/bin/env python
#
# Copyright 2004,2005,2006 Free Software Foundation, Inc.
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

import math
from gnuradio import gr
from gnuradio import audio
from gnuradio.eng_option import eng_option
from optparse import OptionParser

class ofdm_receiver(gr.hier_block):
    def __init__(self, fg, fft_length, symbol_length, snr):
        self.input = gr.add_const_cc(0) # Kluge that goes away with hier_block2

        self.fg = fg
        
        cpsize = symbol_length - fft_length;

        SNR = 10.0**(snr/10.0)
        rho = SNR / (SNR + 1.0)

        # ML Sync

        # Energy Detection from ML Sync

        # Create a delay line
        delayline = [0.0 for i in range(fft_length+1)]
        delayline[fft_length] = 1.0
        self.delay = gr.fir_filter_ccf(1,delayline)
        self.fg.connect(self.input, self.delay)

        # magnitude squared blocks
        self.magsqrd1 = gr.complex_to_mag_squared()
        self.magsqrd2 = gr.complex_to_mag_squared()
        self.adder = gr.add_ff()

        moving_sum_taps = [rho/2 for i in range(cpsize)]
        self.moving_sum_filter = gr.fir_filter_fff(1,moving_sum_taps)
        
        self.fg.connect(self.input,self.magsqrd1)
        self.fg.connect(self.delay,self.magsqrd2)
        self.fg.connect(self.magsqrd1,(self.adder,0))
        self.fg.connect(self.magsqrd2,(self.adder,1))
        self.fg.connect(self.adder,self.moving_sum_filter)
        

        # Correlation from ML Sync
        self.conjg = gr.conjugate_cc();
        self.mixer = gr.multiply_cc();

        movingsum2_taps = [1.0 for i in range(cpsize)]
        self.movingsum2 = gr.fir_filter_ccf(1,movingsum2_taps)
        

        # Correlator data handler
        self.c2mag = gr.complex_to_mag()
        self.angle = gr.complex_to_arg()
        self.fg.connect(self.input,(self.mixer,1))
        self.fg.connect(self.delay,self.conjg,(self.mixer,0))
        self.fg.connect(self.mixer,self.movingsum2,self.c2mag)
        self.fg.connect(self.movingsum2,self.angle)
             
        # ML Sync output arg, need to find maximum point of this
        self.diff = gr.sub_ff()
        self.fg.connect(self.c2mag,(self.diff,0))
        self.fg.connect(self.moving_sum_filter,(self.diff,1))

        #ML measurements input to sampler block and detect
        nco_sensitivity = 1.0/fft_length
        self.f2c = gr.float_to_complex()
        self.sampler = gr.ofdm_sampler(fft_length,symbol_length)
        self.pkt_detect = gr.peak_detector_ff(0.2, 0.25, 30, 0.0001)
        self.dpll = gr.dpll_ff(float(symbol_length),0.01)
        self.sample_and_hold = gr.sample_and_hold_ff()
        self.nco = gr.frequency_modulator_fc(nco_sensitivity)
        self.inv = gr.multiply_const_ff(-1)
        self.sigmix = gr.multiply_cc()

        # Mix the signal with an NCO controlled by the sync loop
        self.fg.connect(self.input, (self.sigmix,0))
        self.fg.connect(self.nco, (self.sigmix,1))
        self.fg.connect(self.sigmix, (self.sampler,0))

        sample_trigger = 0
        if sample_trigger:
            # for testing
            peak_null = gr.null_sink(gr.sizeof_float)
            data = 640*[0,]
            data[639] = 1
            peak_trigger = gr.vector_source_f(data, True)

            self.fg.connect(self.pkt_detect, peak_null)
            self.fg.connect(peak_trigger, self.f2c, (self.sampler,1))
            self.fg.connect(peak_trigger, (self.sample_and_hold,1))
            
        # use the sync loop values to set the sampler and the NCO
        # self.diff = theta
        # self.angle = epsilon
                          
        self.fg.connect(self.diff, self.pkt_detect)
        use_dpll = 1
        if not sample_trigger:
            if use_dpll:
                self.fg.connect(self.pkt_detect, self.dpll,self.f2c, (self.sampler,1))
                self.fg.connect(self.dpll, (self.sample_and_hold,1))
            if not use_dpll:
                self.fg.connect(self.pkt_detect, self.f2c, (self.sampler,1))
                self.fg.connect(self.pkt_detect, (self.sample_and_hold,1))
            
        self.fg.connect(self.angle, (self.sample_and_hold,0))
        self.fg.connect(self.sample_and_hold, self.inv, self.nco)
        

        if 0:
            self.fg.connect(self.diff, gr.file_sink(gr.sizeof_float, "theta_f.dat"))
            self.fg.connect(self.angle, gr.file_sink(gr.sizeof_float, "epsilon_f.dat"))
            if use_dpll:
                self.fg.connect(self.dpll, gr.file_sink(gr.sizeof_float, "dpll_pulses.dat"))
            if sample_trigger:
                self.fg.connect(peak_trigger, gr.file_sink(gr.sizeof_float, "peaks_f.dat"))
            else:
                self.fg.connect(self.pkt_detect, gr.file_sink(gr.sizeof_float, "peaks_f.dat"))
                
            self.fg.connect(self.sample_and_hold, gr.file_sink(gr.sizeof_float, "sample_and_hold_f.dat"))
            self.fg.connect(self.nco, gr.file_sink(gr.sizeof_gr_complex, "nco_c.dat"))
            self.fg.connect(self.input, gr.file_sink(gr.sizeof_gr_complex, "input_c.dat"))
            self.fg.connect(self.sigmix, gr.file_sink(gr.sizeof_gr_complex, "output_c.dat"))
        
        gr.hier_block.__init__(self, fg, self.input, self.sampler)
