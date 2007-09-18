#
# Copyright 2005 Free Software Foundation, Inc.
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
from gnuradio import gr, optfir
from gnuradio.blks2impl.fm_emph import fm_preemph

#from gnuradio import ctcss

class nbfm_tx(gr.hier_block2):
    def __init__(self, audio_rate, quad_rate, tau=75e-6, max_dev=5e3):
        """
        Narrow Band FM Transmitter.

        Takes a single float input stream of audio samples in the range [-1,+1]
        and produces a single FM modulated complex baseband output.

        @param audio_rate: sample rate of audio stream, >= 16k
        @type audio_rate: integer
        @param quad_rate: sample rate of output stream
        @type quad_rate: integer
        @param tau: preemphasis time constant (default 75e-6)
        @type tau: float
        @param max_dev: maximum deviation in Hz (default 5e3)
        @type max_dev: float

        quad_rate must be an integer multiple of audio_rate.
        """

	gr.hier_block2.__init__(self, "nbfm_tx",
				gr.io_signature(1, 1, gr.sizeof_float),      # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature
        
        # FIXME audio_rate and quad_rate ought to be exact rationals
        audio_rate = int(audio_rate)
        quad_rate = int(quad_rate)

        if quad_rate % audio_rate != 0:
            raise ValueError, "quad_rate is not an integer multiple of audio_rate"

        
        do_interp = audio_rate != quad_rate
        
        if do_interp:
            interp_factor = quad_rate / audio_rate
            interp_taps = optfir.low_pass (interp_factor,   # gain
                                           quad_rate,       # Fs
                                           4500,            # passband cutoff
                                           7000,            # stopband cutoff
                                           0.1,  	    # passband ripple dB
                                           40)              # stopband atten dB

            #print "len(interp_taps) =", len(interp_taps)
            self.interpolator = gr.interp_fir_filter_fff (interp_factor, interp_taps)

        self.preemph = fm_preemph (quad_rate, tau=tau)
        
        k = 2 * math.pi * max_dev / quad_rate
        self.modulator = gr.frequency_modulator_fc (k)

        if do_interp:
            self.connect (self, self.interpolator, self.preemph, self.modulator, self)
        else:
            self.connect(self, self.preemph, self.modulator, self)
        
       
class ctcss_gen_f(gr.hier_block2):
    def __init__(self, sample_rate, tone_freq):
	gr.hier_block2.__init__(self, "ctcss_gen_f",
				gr.io_signature(0, 0, 0),               # Input signature
				gr.io_signature(1, 1, gr.sizeof_float)) # Output signature

        self.plgen = gr.sig_source_f(sample_rate, gr.GR_SIN_WAVE, tone_freq, 0.1, 0.0)
	self.connect(self.plgen, self)        
