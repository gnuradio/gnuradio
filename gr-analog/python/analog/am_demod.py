#
# Copyright 2006,2007,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import blocks
from gnuradio import filter


class am_demod_cf(gr.hier_block2):
    """
    Generalized AM demodulation block with audio filtering.

    This block demodulates a band-limited, complex down-converted AM
    channel into the the original baseband signal, applying low pass
    filtering to the audio output. It produces a float stream in the
    range [-1.0, +1.0].

    Args:
        channel_rate: incoming sample rate of the AM baseband (integer)
        audio_decim: input to output decimation rate (integer)
        audio_pass: audio low pass filter passband frequency (float)
        audio_stop: audio low pass filter stop frequency (float)
    """

    def __init__(self, channel_rate, audio_decim, audio_pass, audio_stop):
        gr.hier_block2.__init__(self, "am_demod_cf",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(1, 1, gr.sizeof_float))      # Input signature

        MAG = blocks.complex_to_mag()
        DCR = blocks.add_const_ff(-1.0)

        audio_taps = filter.optfir.low_pass(0.5,          # Filter gain
                                            channel_rate,  # Sample rate
                                            audio_pass,   # Audio passband
                                            audio_stop,   # Audio stopband
                                            0.1,          # Passband ripple
                                            60)           # Stopband attenuation
        LPF = filter.fir_filter_fff(audio_decim, audio_taps)

        self.connect(self, MAG, DCR, LPF, self)


class demod_10k0a3e_cf(am_demod_cf):
    """
    AM demodulation block, 10 KHz channel.

    This block demodulates an AM channel conformant to 10K0A3E emission
    standards, such as broadcast band AM transmissions.

    Args:
        channel_rate: incoming sample rate of the AM baseband (integer)
        audio_decim: input to output decimation rate (integer)
    """

    def __init__(self, channel_rate, audio_decim):
        am_demod_cf.__init__(self, channel_rate, audio_decim,
                             5000,  # Audio passband
                             5500)  # Audio stopband
