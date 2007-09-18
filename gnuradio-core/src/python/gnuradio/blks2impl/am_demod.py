#
# Copyright 2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, optfir

class am_demod_cf(gr.hier_block2):
    """
    Generalized AM demodulation block with audio filtering.

    This block demodulates a band-limited, complex down-converted AM 
    channel into the the original baseband signal, applying low pass 
    filtering to the audio output. It produces a float stream in the
    range [-1.0, +1.0].

    @param channel_rate:  incoming sample rate of the AM baseband
    @type sample_rate: integer
    @param audio_decim: input to output decimation rate
    @type audio_decim: integer
    @param audio_pass: audio low pass filter passband frequency 
    @type audio_pass: float
    @param audio_stop: audio low pass filter stop frequency
    @type audio_stop: float
    """ 
    def __init__(self, channel_rate, audio_decim, audio_pass, audio_stop):
	gr.hier_block2.__init__(self, "am_demod_cf",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_float))      # Input signature

	MAG = gr.complex_to_mag()
	DCR = gr.add_const_ff(-1.0)

	audio_taps = optfir.low_pass(0.5, 	   # Filter gain
	                             channel_rate, # Sample rate
				     audio_pass,   # Audio passband
				     audio_stop,   # Audio stopband
				     0.1, 	   # Passband ripple
				     60)	   # Stopband attenuation
	LPF = gr.fir_filter_fff(audio_decim, audio_taps)

	self.connect(self, MAG, DCR, LPF, self)

class demod_10k0a3e_cf(am_demod_cf):
    """
    AM demodulation block, 10 KHz channel.
    
    This block demodulates an AM channel conformant to 10K0A3E emission
    standards, such as broadcast band AM transmissions.

    @param channel_rate:  incoming sample rate of the AM baseband
    @type sample_rate: integer
    @param audio_decim: input to output decimation rate
    @type audio_decim: integer
    """
    def __init__(self, channel_rate, audio_decim):
    	am_demod_cf.__init__(self, channel_rate, audio_decim, 
    	                     5000, # Audio passband
    	                     5500) # Audio stopband
    	