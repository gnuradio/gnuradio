#
# Copyright 2006,2007,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, filter
from .fm_emph import fm_deemph
from math import pi

from . import analog_python as analog


class fm_demod_cf(gr.hier_block2):
    """
    Generalized FM demodulation block with deemphasis and audio
    filtering.

    This block demodulates a band-limited, complex down-converted FM
    channel into the the original baseband signal, optionally applying
    deemphasis. Low pass filtering is done on the resultant signal. It
    produces an output float stream in the range of [-1.0, +1.0].

    Args:
        channel_rate: incoming sample rate of the FM baseband (integer)
        deviation: maximum FM deviation (default = 5000) (float)
        audio_decim: input to output decimation rate (integer)
        audio_pass: audio low pass filter passband frequency (float)
        audio_stop: audio low pass filter stop frequency (float)
        gain: gain applied to audio output (default = 1.0) (float)
        tau: deemphasis time constant (default = 75e-6), specify tau=0.0 to prevent deemphasis (float)
    """

    def __init__(self, channel_rate, audio_decim, deviation,
                 audio_pass, audio_stop, gain=1.0, tau=75e-6):
        gr.hier_block2.__init__(self, "fm_demod_cf",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(1, 1, gr.sizeof_float))       # Output signature

        k = channel_rate / (2 * pi * deviation)
        QUAD = analog.quadrature_demod_cf(k)

        audio_taps = filter.optfir.low_pass(
            gain,          # Filter gain
            channel_rate,  # Sample rate
            audio_pass,    # Audio passband
            audio_stop,    # Audio stopband
            0.1,           # Passband ripple
            60             # Stopband attenuation
        )
        LPF = filter.fir_filter_fff(audio_decim, audio_taps)

        if tau is not None and tau > 0.0:  # None should be deprecated someday
            DEEMPH = fm_deemph(channel_rate, tau)
            self.connect(self, QUAD, DEEMPH, LPF, self)
        else:
            self.connect(self, QUAD, LPF, self)


class demod_20k0f3e_cf(fm_demod_cf):
    """
    NBFM demodulation block, 20 KHz channels

    This block demodulates a complex, downconverted, narrowband FM
    channel conforming to 20K0F3E emission standards, outputting
    floats in the range [-1.0, +1.0].

    Args:
        sample_rate: incoming sample rate of the FM baseband (integer)
        audio_decim: input to output decimation rate (integer)
    """

    def __init__(self, channel_rate, audio_decim):
        fm_demod_cf.__init__(self, channel_rate, audio_decim,
                             5000,  # Deviation
                             3000,  # Audio passband frequency
                             4500)  # Audio stopband frequency


class demod_200kf3e_cf(fm_demod_cf):
    """
    WFM demodulation block, mono.

    This block demodulates a complex, downconverted, wideband FM
    channel conforming to 200KF3E emission standards, outputting
    floats in the range [-1.0, +1.0].

    Args:
        sample_rate: incoming sample rate of the FM baseband (integer)
        audio_decim: input to output decimation rate (integer)
    """

    def __init__(self, channel_rate, audio_decim):
        fm_demod_cf.__init__(self, channel_rate, audio_decim,
                             75000,  # Deviation
                             15000,  # Audio passband
                             16000)  # Audio stopband
