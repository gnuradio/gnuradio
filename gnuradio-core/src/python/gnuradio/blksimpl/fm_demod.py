#
# Copyright 2006 Free Software Foundation, Inc.
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
from gnuradio.blksimpl.fm_emph import fm_deemph
from math import pi

class fm_demod_cf(gr.hier_block):
    """
    Generalized FM demodulation block with deemphasis and audio 
    filtering.

    This block demodulates a band-limited, complex down-converted FM 
    channel into the the original baseband signal, optionally applying
    deemphasis. Low pass filtering is done on the resultant signal. It
    produces an output float strem in the range of [-1.0, +1.0].

    @param fg: flowgraph
    @param channel_rate:  incoming sample rate of the FM baseband
    @type sample_rate: integer
    @param deviation: maximum FM deviation (default = 5000)
    @type deviation: float
    @param audio_decim: input to output decimation rate
    @type audio_decim: integer
    @param audio_pass: audio low pass filter passband frequency 
    @type audio_pass: float
    @param audio_stop: audio low pass filter stop frequency
    @type audio_stop: float
    @param gain: gain applied to audio output (default = 1.0)
    @type gain: float
    @param tau: deemphasis time constant (default = 75e-6), specify 'None'
       to prevent deemphasis
    """ 
    def __init__(self, fg, channel_rate, audio_decim, deviation, 
                 audio_pass, audio_stop, gain=1.0, tau=75e-6):

        """	
	# Equalizer for ~100 us delay
	delay = 100e-6
	num_taps = int(channel_rate*delay)

	mu = 1e-4/num_taps
	print "CMA: delay =", delay, "n =", num_taps, "mu =", mu
	CMA = gr.cma_equalizer_cc(num_taps, 1.0, mu)
        """	
	k = channel_rate/(2*pi*deviation)
	QUAD = gr.quadrature_demod_cf(k)

	audio_taps = optfir.low_pass(gain, 	   # Filter gain
	                             channel_rate, # Sample rate
				     audio_pass,   # Audio passband
				     audio_stop,   # Audio stopband
				     0.1, 	   # Passband ripple
				     60)	   # Stopband attenuation
	LPF = gr.fir_filter_fff(audio_decim, audio_taps)

	if tau is not None:
	    DEEMPH = fm_deemph(fg, channel_rate, tau)
	    fg.connect(QUAD, DEEMPH, LPF)
        else:
            fg.connect(QUAD, LPF)

        gr.hier_block.__init__(self, fg, QUAD, LPF)

class demod_20k0f3e_cf(fm_demod_cf):
    """
    NBFM demodulation block, 20 KHz channels

    This block demodulates a complex, downconverted, narrowband FM 
    channel conforming to 20K0F3E emission standards, outputting
    floats in the range [-1.0, +1.0].
    
    @param fg: flowgraph
    @param sample_rate:  incoming sample rate of the FM baseband
    @type sample_rate: integer
    @param audio_decim: input to output decimation rate
    @type audio_decim: integer
    """ 
    def __init__(self, fg, channel_rate, audio_decim):
        fm_demod_cf.__init__(self, fg, channel_rate, audio_decim,
                             5000,	# Deviation
                             3000,	# Audio passband frequency
                             4000)	# Audio stopband frequency

class demod_200kf3e_cf(fm_demod_cf):
    """
    WFM demodulation block, mono.
    
    This block demodulates a complex, downconverted, wideband FM 
    channel conforming to 200KF3E emission standards, outputting 
    floats in the range [-1.0, +1.0].

    @param fg: flowgraph
    @param sample_rate:  incoming sample rate of the FM baseband
    @type sample_rate: integer
    @param audio_decim: input to output decimation rate
    @type audio_decim: integer
    """
    def __init__(self, fg, channel_rate, audio_decim): 
	fm_demod_cf.__init__(self, fg, channel_rate, audio_decim,
			     75000,	# Deviation
			     15000,	# Audio passband
			     16000,	# Audio stopband
			     20.0)	# Audio gain
