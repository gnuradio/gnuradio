#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2013 Sylvain Munaut <tnt@246tNt.com>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import math
import cmath

from gnuradio import gr
from gnuradio.blocks import rotator_cc

from filter_swig import fft_filter_ccc


__all__ = [ 'freq_xlating_fft_filter_ccc' ]


class freq_xlating_fft_filter_ccc(gr.hier_block2):

    def __init__(self, decim, taps, center_freq, samp_rate):
        gr.hier_block2.__init__(
            self,
            'freq_xlating_fft_filter_ccc',
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
        )

        # Save args
        self.decim       = decim
        self.taps        = taps
        self.center_freq = center_freq
        self.samp_rate   = samp_rate

        # Sub blocks
        self._filter = fft_filter_ccc(decim, taps)
        self._rotator = rotator_cc(0.0)

        self.connect(self, self._filter, self._rotator, self)

        # Refresh
        self._refresh()

    def _rotate_taps(self, taps, phase_inc):
        return [ x * cmath.exp(i * phase_inc * 1j) for i,x in enumerate(taps) ]

    def _refresh(self):
        phase_inc = (2.0 * math.pi * self.center_freq) / self.samp_rate
        rtaps = self._rotate_taps(self.taps, phase_inc)
        self._filter.set_taps(rtaps)
        self._rotator.set_phase_inc(- self.decim * phase_inc)

    def set_taps(self, taps):
        self.taps = taps
        self._refresh()

    def set_center_freq(self, center_freq):
        self.center_freq = center_freq
        self._refresh()

    def set_nthreads(self, nthreads):
        self._filter.set_nthreads(nthreads)

    def declare_sample_delay(self, samp_delay):
        self._filter.declare_sample_delay(samp_delay)
