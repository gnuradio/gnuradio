#
# Copyright 2005,2007 Free Software Foundation, Inc.
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

from gnuradio import gr
from gnuradio.blks2impl.fm_emph import fm_deemph
import math

class wfm_rcv(gr.hier_block2):
    def __init__ (self, quad_rate, audio_decimation):
        """
        Hierarchical block for demodulating a broadcast FM signal.
        
        The input is the downconverted complex baseband signal (gr_complex).
        The output is the demodulated audio (float).
        
        @param quad_rate: input sample rate of complex baseband input.
        @type quad_rate: float
        @param audio_decimation: how much to decimate quad_rate to get to audio.
        @type audio_decimation: integer
        """
	gr.hier_block2.__init__(self, "wfm_rcv",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_float))      # Output signature

        volume = 20.

        max_dev = 75e3
        fm_demod_gain = quad_rate/(2*math.pi*max_dev)
        audio_rate = quad_rate / audio_decimation
        

        # We assign to self so that outsiders can grab the demodulator 
        # if they need to.  E.g., to plot its output.
        #
        # input: complex; output: float
        self.fm_demod = gr.quadrature_demod_cf (fm_demod_gain)

        # input: float; output: float
        self.deemph = fm_deemph (audio_rate)
        
        # compute FIR filter taps for audio filter
        width_of_transition_band = audio_rate / 32
        audio_coeffs = gr.firdes.low_pass (1.0,         # gain
                                           quad_rate,      # sampling rate
                                           audio_rate/2 - width_of_transition_band,
                                           width_of_transition_band,
                                           gr.firdes.WIN_HAMMING)
        # input: float; output: float
        self.audio_filter = gr.fir_filter_fff (audio_decimation, audio_coeffs)

        self.connect (self, self.fm_demod, self.audio_filter, self.deemph, self)
