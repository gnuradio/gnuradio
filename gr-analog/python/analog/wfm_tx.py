#
# Copyright 2005,2007,2012,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math

from gnuradio import gr
from gnuradio import filter

from . import analog_python as analog
from .fm_emph import fm_preemph


class wfm_tx(gr.hier_block2):
    def __init__(self, audio_rate, quad_rate, tau=75e-6, max_dev=75e3, fh=-1.0):
        """
        Wide Band FM Transmitter.

        Takes a single float input stream of audio samples in the range [-1,+1]
        and produces a single FM modulated complex baseband output.

        Args:
            audio_rate: sample rate of audio stream, >= 16k (integer)
            quad_rate: sample rate of output stream (integer)
            tau: preemphasis time constant (default 75e-6) (float)
            max_dev: maximum deviation in Hz (default 75e3) (float)
            fh: high frequency at which to flatten preemphasis; < 0 means default of 0.925*quad_rate/2.0 (float)

        quad_rate must be an integer multiple of audio_rate.
        """
        gr.hier_block2.__init__(self, "wfm_tx",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(1, 1, gr.sizeof_gr_complex))  # Output signature

        # FIXME audio_rate and quad_rate ought to be exact rationals
        audio_rate = int(audio_rate)
        quad_rate = int(quad_rate)

        if quad_rate % audio_rate != 0:
            raise ValueError(
                "quad_rate is not an integer multiple of audio_rate")

        do_interp = audio_rate != quad_rate

        if do_interp:
            interp_factor = quad_rate // audio_rate
            interp_taps = filter.firdes.low_pass(interp_factor,  # gain
                                                 quad_rate,      # Fs
                                                 19000,          # cutoff_freq
                                                 4000)           # transition_width
            print("len(interp_taps) =", len(interp_taps))
            self.interpolator = filter.interp_fir_filter_fff(
                interp_factor, interp_taps)

        self.preemph = fm_preemph(quad_rate, tau=tau, fh=fh)

        k = 2 * math.pi * max_dev / quad_rate
        self.modulator = analog.frequency_modulator_fc(k)

        if do_interp:
            self.connect(self, self.interpolator,
                         self.preemph, self.modulator, self)
        else:
            self.connect(self, self.preemph, self.modulator, self)
