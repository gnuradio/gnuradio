#
# Copyright 2008 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
from gnuradio import blocks
import sys
import math

from . import fft_python as fft
from . import fft_vfc, fft_vcc
from .fft_python import window

try:
    from gnuradio import filter
except ImportError:
    sys.stderr.write('fft.logpwrfft required gr-filter.\n')
    sys.exit(1)


class _logpwrfft_base(gr.hier_block2):
    """
    Create a log10(abs(fft)) stream chain, with real or complex input.
    """

    def __init__(self, sample_rate, fft_size, ref_scale, frame_rate, avg_alpha, average, win=None, shift=False):
        """
        Create an log10(abs(fft)) stream chain.
        Provide access to the setting the filter and sample rate.

        Args:
            sample_rate: Incoming stream sample rate
            fft_size: Number of FFT bins
            ref_scale: Sets 0 dB value input amplitude
            frame_rate: Output frame rate
            avg_alpha: FFT averaging (over time) constant [0.0-1.0]
            average: Whether to average [True, False]
            win: the window taps generation function
            shift: shift zero-frequency component to center of spectrum
        """
        gr.hier_block2.__init__(self, self._name,
                                # Input signature
                                gr.io_signature(1, 1, self._item_size),
                                gr.io_signature(1, 1, gr.sizeof_float * fft_size))  # Output signature

        self._sd = blocks.stream_to_vector_decimator(item_size=self._item_size,
                                                     sample_rate=sample_rate,
                                                     vec_rate=frame_rate,
                                                     vec_len=fft_size)
        if win is None:
            win = window.blackmanharris
        fft_window = win(fft_size)
        fft = self._fft_block[0](fft_size, True, fft_window, shift=shift)
        window_power = sum([x * x for x in fft_window])

        c2magsq = blocks.complex_to_mag_squared(fft_size)
        self._avg = filter.single_pole_iir_filter_ff(1.0, fft_size)
        self._log = blocks.nlog10_ff(10, fft_size,
                                     # Adjust for number of bins
                                     -20 * math.log10(fft_size) -
                                     # Adjust for windowing loss
                                     10 * math.log10(float(window_power) / fft_size) -
                                     20 * math.log10(float(ref_scale) / 2))      # Adjust for reference scale
        self.connect(self, self._sd, fft, c2magsq, self._avg, self._log, self)

        self._average = average
        self._avg_alpha = avg_alpha
        self.set_avg_alpha(avg_alpha)
        self.set_average(average)

    def set_decimation(self, decim):
        """
        Set the decimation on stream decimator.

        Args:
            decim: the new decimation
        """
        self._sd.set_decimation(decim)

    def set_vec_rate(self, vec_rate):
        """
        Set the vector rate on stream decimator.

        Args:
            vec_rate: the new vector rate
        """
        self._sd.set_vec_rate(vec_rate)

    def set_sample_rate(self, sample_rate):
        """
        Set the new sampling rate

        Args:
            sample_rate: the new rate
        """
        self._sd.set_sample_rate(sample_rate)

    def set_average(self, average):
        """
        Set the averaging filter on/off.

        Args:
            average: true to set averaging on
        """
        self._average = average
        if self._average:
            self._avg.set_taps(self._avg_alpha)
        else:
            self._avg.set_taps(1.0)

    def set_avg_alpha(self, avg_alpha):
        """
        Set the average alpha and set the taps if average was on.

        Args:
            avg_alpha: the new iir filter tap
        """
        self._avg_alpha = avg_alpha
        self.set_average(self._average)

    def sample_rate(self):
        """
        Return the current sample rate.
        """
        return self._sd.sample_rate()

    def decimation(self):
        """
        Return the current decimation.
        """
        return self._sd.decimation()

    def frame_rate(self):
        """
        Return the current frame rate.
        """
        return self._sd.frame_rate()

    def average(self):
        """
        Return whether or not averaging is being performed.
        """
        return self._average

    def avg_alpha(self):
        """
        Return averaging filter constant.
        """
        return self._avg_alpha


class logpwrfft_f(_logpwrfft_base):
    """
    Create an fft block chain, with real input.
    """
    _name = "logpwrfft_f"
    _item_size = gr.sizeof_float
    _fft_block = (fft_vfc, )


class logpwrfft_c(_logpwrfft_base):
    """
    Create an fft block chain, with complex input.
    """
    _name = "logpwrfft_c"
    _item_size = gr.sizeof_gr_complex
    _fft_block = (fft_vcc, )
