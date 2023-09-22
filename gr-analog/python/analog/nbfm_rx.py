#
# Copyright 2005,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math

from gnuradio import gr
from gnuradio import filter, fft

from . import analog_python as analog
from .fm_emph import fm_deemph


class nbfm_rx(gr.hier_block2):
    """
    Narrow Band FM Receiver.

    Takes a single complex baseband input stream and produces a single
    float output stream of audio sample in the range [-1, +1].

    Args:
        audio_rate: sample rate of audio stream, >= 16k (integer)
        quad_rate: sample rate of output stream (integer)
        tau: preemphasis time constant (default 75e-6) (float)
        max_dev: maximum deviation in Hz (default 5e3) (float)

    quad_rate must be an integer multiple of audio_rate.

    Exported sub-blocks (attributes):
      squelch
      quad_demod
      deemph
      audio_filter
    """

    def __init__(self, audio_rate, quad_rate, tau=75e-6, max_dev=5e3):
        gr.hier_block2.__init__(self, "nbfm_rx",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(1, 1, gr.sizeof_float))      # Output signature

        # FIXME audio_rate and quad_rate ought to be exact rationals
        self._audio_rate = audio_rate = int(audio_rate)
        self._quad_rate = quad_rate = int(quad_rate)

        if quad_rate % audio_rate != 0:
            raise ValueError(
                "quad_rate is not an integer multiple of audio_rate")

        squelch_threshold = 20        # dB
        #self.squelch = analog.simple_squelch_cc(squelch_threshold, 0.001)

        # FM Demodulator  input: complex; output: float
        k = quad_rate / (2 * math.pi * max_dev)
        self.quad_demod = analog.quadrature_demod_cf(k)

        # FM Deemphasis IIR filter
        self.deemph = fm_deemph(quad_rate, tau=tau)

        # compute FIR taps for audio filter
        audio_decim = quad_rate // audio_rate
        audio_taps = filter.firdes.low_pass(1.0,            # gain
                                            quad_rate,      # sampling rate
                                            2.7e3,          # Audio LPF cutoff
                                            0.5e3,          # Transition band
                                            fft.window.WIN_HAMMING)  # filter type

        print("len(audio_taps) =", len(audio_taps))

        # Decimating audio filter
        # input: float; output: float; taps: float
        self.audio_filter = filter.fir_filter_fff(audio_decim, audio_taps)

        self.connect(self, self.quad_demod, self.deemph,
                     self.audio_filter, self)

    def set_max_deviation(self, max_dev):
        k = self._quad_rate / (2 * math.pi * max_dev)
        self.quad_demod.set_gain(k)
