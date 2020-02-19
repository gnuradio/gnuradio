#!/usr/bin/env python
#
# Copyright 2007,2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from __future__ import division
from __future__ import unicode_literals
from gnuradio import gr, filter, blocks
from . import vocoder_swig


class cvsd_encode_fb(gr.hier_block2):
    '''
    This is a wrapper for the CVSD encoder that performs interpolation and filtering
    necessary to work with the vocoding. It converts an incoming float (+-1) to a short, scales
    it (to 32000; slightly below the maximum value), interpolates it, and then vocodes it.

    The incoming sampling rate can be anything, though, of course, the higher the sampling rate and the
    higher the interpolation rate are, the better the sound quality.
    '''

    def __init__(self, resample=8, bw=0.5):
        '''
        When using the CVSD vocoder, appropriate sampling rates are from 8k to 64k with resampling rates
        from 1 to 8. A rate of 8k with a resampling rate of 8 provides a good quality signal.
        '''

        gr.hier_block2.__init__(self, "cvsd_encode",
                                gr.io_signature(1, 1, gr.sizeof_float), # Input signature
                                gr.io_signature(1, 1, gr.sizeof_char))  # Output signature

        scale_factor = 32000.0
        self.interp = resample

        src_scale = blocks.multiply_const_ff(scale_factor)
        taps = filter.firdes.low_pass(self.interp, self.interp, bw, 2*bw)
        interp = filter.interp_fir_filter_fff(self.interp, taps)
        f2s = blocks.float_to_short()
        enc = vocoder_swig.cvsd_encode_sb()

        self.connect(self, src_scale, interp, f2s, enc, self)


class cvsd_decode_bf(gr.hier_block2):
    '''
    This is a wrapper for the CVSD decoder that performs decimation and filtering
    necessary to work with the vocoding. It converts an incoming CVSD-encoded short to a float, decodes it
    to a float, decimates it, and scales it (by 32000; slightly below the maximum value to avoid clipping).

    The sampling rate can be anything, though, of course, the higher the sampling rate and the
    higher the interpolation rate are, the better the sound quality.
    '''

    def __init__(self, resample=8, bw=0.5):
        '''
        When using the CVSD vocoder, appropriate sampling rates are from 8k to 64k with resampling rates
        from 1 to 8. A rate of 8k with a resampling rate of 8 provides a good quality signal.
        '''
        gr.hier_block2.__init__(self, "cvsd_decode",
                                gr.io_signature(1, 1, gr.sizeof_char),  # Input signature
                                gr.io_signature(1, 1, gr.sizeof_float)) # Output signature

        scale_factor = 32000.0
        self.decim = resample

        dec = vocoder_swig.cvsd_decode_bs()
        s2f = blocks.short_to_float()
        taps = filter.firdes.low_pass(1, 1, bw, 2*bw)
        decim = filter.fir_filter_fff(self.decim, taps)
        sink_scale = blocks.multiply_const_ff(1.0 / scale_factor)

        self.connect(self, dec, s2f, decim, sink_scale, self)
