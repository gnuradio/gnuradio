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
from gnuradio.blks2impl.fm_emph import fm_deemph
#from gnuradio.blks2impl.standard_squelch import standard_squelch

class nbfm_rx(gr.hier_block2):
    def __init__(self, audio_rate, quad_rate, tau=75e-6, max_dev=5e3):
        """
        Narrow Band FM Receiver.

        Takes a single complex baseband input stream and produces a single
        float output stream of audio sample in the range [-1, +1].

        @param audio_rate: sample rate of audio stream, >= 16k
        @type audio_rate: integer
        @param quad_rate: sample rate of output stream
        @type quad_rate: integer
        @param tau: preemphasis time constant (default 75e-6)
        @type tau: float
        @param max_dev: maximum deviation in Hz (default 5e3)
        @type max_dev: float

        quad_rate must be an integer multiple of audio_rate.

        Exported sub-blocks (attributes):
          squelch
          quad_demod
          deemph
          audio_filter
        """

	gr.hier_block2.__init__(self, "nbfm_rx",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(1, 1, gr.sizeof_float))      # Output signature

        # FIXME audio_rate and quad_rate ought to be exact rationals
        audio_rate = int(audio_rate)
        quad_rate = int(quad_rate)

        if quad_rate % audio_rate != 0:
            raise ValueError, "quad_rate is not an integer multiple of audio_rate"

        squelch_threshold = 20		# dB
        #self.squelch = gr.simple_squelch_cc(squelch_threshold, 0.001)

        # FM Demodulator  input: complex; output: float
        k = quad_rate/(2*math.pi*max_dev)
        self.quad_demod = gr.quadrature_demod_cf(k)

        # FM Deemphasis IIR filter
        self.deemph = fm_deemph (quad_rate, tau=tau)

        # compute FIR taps for audio filter
        audio_decim = quad_rate // audio_rate
        audio_taps = gr.firdes.low_pass (1.0,            # gain
                                         quad_rate,      # sampling rate
                                         2.7e3,          # Audio LPF cutoff
                                         0.5e3,          # Transition band
                                         gr.firdes.WIN_HAMMING)  # filter type

        print "len(audio_taps) =", len(audio_taps)

        # Decimating audio filter
        # input: float; output: float; taps: float
        self.audio_filter = gr.fir_filter_fff(audio_decim, audio_taps)

        self.connect(self, self.quad_demod, self.deemph, self.audio_filter, self)
