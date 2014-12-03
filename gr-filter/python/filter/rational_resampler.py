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

from gnuradio import gr, gru
import filter_swig as filter

_plot = None

def design_filter(interpolation, decimation, fractional_bw):
    """
    Given the interpolation rate, decimation rate and a fractional bandwidth,
    design a set of taps.

    Args:
        interpolation: interpolation factor (integer > 0)
        decimation: decimation factor (integer > 0)
        fractional_bw: fractional bandwidth in (0, 0.5)  0.4 works well. (float)
    Returns:
        : sequence of numbers
    """

    if fractional_bw >= 0.5 or fractional_bw <= 0:
        raise ValueError, "Invalid fractional_bandwidth, must be in (0, 0.5)"

    beta = 7.0
    halfband = 0.5
    rate = float(interpolation)/float(decimation)
    if(rate >= 1.0):
        trans_width = halfband - fractional_bw
        mid_transition_band = halfband - trans_width/2.0
    else:
        trans_width = rate*(halfband - fractional_bw)
        mid_transition_band = rate*halfband - trans_width/2.0

    taps = filter.firdes.low_pass(interpolation,                     # gain
                                  interpolation,                     # Fs
                                  mid_transition_band,               # trans mid point
                                  trans_width,                       # transition width
                                  filter.firdes.WIN_KAISER,
                                  beta)                              # beta

    return taps



class _rational_resampler_base(gr.hier_block2):
    """
    base class for all rational resampler variants.
    """
    def __init__(self, resampler_base,
                 interpolation, decimation, taps=None, fractional_bw=None):
        """
        Rational resampling polyphase FIR filter.

        Either taps or fractional_bw may be specified, but not both.
        If neither is specified, a reasonable default, 0.4, is used as
        the fractional_bw.

        Args:
            interpolation: interpolation factor (integer > 0)
            decimation: decimation factor (integer > 0)
            taps: optional filter coefficients (sequence)
            fractional_bw: fractional bandwidth in (0, 0.5), measured at final freq (use 0.4) (float)
        """

        if not isinstance(interpolation, int) or interpolation < 1:
            raise ValueError, "interpolation must be an integer >= 1"

        if not isinstance(decimation, int) or decimation < 1:
            raise ValueError, "decimation must be an integer >= 1"

        if taps is None and fractional_bw is None:
            fractional_bw = 0.4

        d = gru.gcd(interpolation, decimation)

        # If we have user-provided taps and the interp and decim
        # values have a common divisor, we don't reduce these values
        # by the GCD but issue a warning to the user that this might
        # increase the complexity of the filter.
        if taps and (d > 1):
            gr.log.info("Rational resampler has user-provided taps but interpolation ({0}) and decimation ({1}) have a GCD of {2}, which increases the complexity of the filterbank. Consider reducing these values by the GCD.".format(interpolation, decimation, d))

        # If we don't have user-provided taps, reduce the interp and
        # decim values by the GCD (if there is one) and then define
        # the taps from these new values.
        if taps is None:
            interpolation = interpolation // d
            decimation = decimation // d
            taps = design_filter(interpolation, decimation, fractional_bw)

        self.resampler = resampler_base(interpolation, decimation, taps)
	gr.hier_block2.__init__(self, "rational_resampler",
				gr.io_signature(1, 1, self.resampler.input_signature().sizeof_stream_item(0)),
				gr.io_signature(1, 1, self.resampler.output_signature().sizeof_stream_item(0)))

	self.connect(self, self.resampler, self)

    def taps(self):
        return self.resampler.taps()

class rational_resampler_fff(_rational_resampler_base):
    def __init__(self, interpolation, decimation, taps=None, fractional_bw=None):
        """
        Rational resampling polyphase FIR filter with
        float input, float output and float taps.
        """
        _rational_resampler_base.__init__(self, filter.rational_resampler_base_fff,
				          interpolation, decimation, taps, fractional_bw)

class rational_resampler_ccf(_rational_resampler_base):
    def __init__(self, interpolation, decimation, taps=None, fractional_bw=None):
        """
        Rational resampling polyphase FIR filter with
        complex input, complex output and float taps.
        """
        _rational_resampler_base.__init__(self, filter.rational_resampler_base_ccf,
                                          interpolation, decimation, taps, fractional_bw)

class rational_resampler_ccc(_rational_resampler_base):
    def __init__(self, interpolation, decimation, taps=None, fractional_bw=None):
        """
        Rational resampling polyphase FIR filter with
        complex input, complex output and complex taps.
        """
        _rational_resampler_base.__init__(self, filter.rational_resampler_base_ccc,
                                          interpolation, decimation, taps, fractional_bw)
