#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: FM stereo demod block

from __future__ import absolute_import
from __future__ import division
from __future__ import unicode_literals

import math

from gnuradio import gr
from gnuradio import blocks
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio import analog

class wfm_rcv_pll(gr.hier_block2):
    def __init__(self, demod_rate, audio_decimation):
        """
        Hierarchical block for demodulating a broadcast FM signal.

        The input is the downconverted complex baseband signal (gr_complex).
        The output is two streams of the demodulated audio (float) 0=Left, 1=Right.

        Args:
            demod_rate: input sample rate of complex baseband input. (float)
            audio_decimation: how much to decimate demod_rate to get to audio. (integer)
        """
        gr.hier_block2.__init__(self, "wfm_rcv_pll",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
                                gr.io_signature(2, 2, gr.sizeof_float))      # Output signature

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 3840000
        self.rf_decim = rf_decim = 10
        self.demod_rate = demod_rate = (int)(samp_rate/rf_decim)
        self.stereo_carrier_filter_coeffs_0 = stereo_carrier_filter_coeffs_0 = firdes.band_pass(1.0, demod_rate, 37600, 38400, 400, firdes.WIN_HAMMING, 6.76)
        self.stereo_carrier_filter_coeffs = stereo_carrier_filter_coeffs = firdes.complex_band_pass(1.0, demod_rate, 18980, 19020, 1500, firdes.WIN_HAMMING, 6.76)
        self.deviation = deviation = 75000
        self.audio_filter = audio_filter = firdes.low_pass(1, demod_rate, 15000,1500, firdes.WIN_HAMMING, 6.76)
        self.audio_decim = audio_decim = (int)(demod_rate/48000)

        ##################################################
        # Blocks
        ##################################################
        self.fir_filter_xxx_1 = filter.fir_filter_fcc(1, stereo_carrier_filter_coeffs)
        self.fir_filter_xxx_1.declare_sample_delay(0)
        self.fft_filter_xxx_3 = filter.fft_filter_fff(1, stereo_carrier_filter_coeffs_0, 1)
        self.fft_filter_xxx_3.declare_sample_delay(0)
        self.fft_filter_xxx_2 = filter.fft_filter_fff(audio_decim, audio_filter, 1)
        self.fft_filter_xxx_2.declare_sample_delay(0)
        self.fft_filter_xxx_1 = filter.fft_filter_fff(audio_decim, audio_filter, 1)
        self.fft_filter_xxx_1.declare_sample_delay(0)
        self.blocks_multiply_xx_2 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_0_1 = blocks.multiply_const_ff(5.5)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_ff(-5.5)
        self.blocks_complex_to_imag_0 = blocks.complex_to_imag(1)
        self.blocks_add_xx_0_0 = blocks.add_vff(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.analog_quadrature_demod_cf_0 = analog.quadrature_demod_cf(demod_rate/(2*math.pi*deviation))
        self.analog_pll_refout_cc_0 = analog.pll_refout_cc(0.001, 2*math.pi * 19200 / demod_rate, 2*math.pi * 18800 / demod_rate)
        self.analog_fm_deemph_0_0 = analog.fm_deemph(fs=48000, tau=75e-6)
        self.analog_fm_deemph_0 = analog.fm_deemph(fs=48000, tau=75e-6)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_fm_deemph_0, 0), (self, 1))
        self.connect((self.analog_fm_deemph_0_0, 0), (self, 0))
        self.connect((self.analog_pll_refout_cc_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_pll_refout_cc_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.blocks_multiply_xx_2, 0))
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.fft_filter_xxx_1, 0))
        self.connect((self.analog_quadrature_demod_cf_0, 0), (self.fir_filter_xxx_1, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.analog_fm_deemph_0, 0))
        self.connect((self.blocks_add_xx_0_0, 0), (self.analog_fm_deemph_0_0, 0))
        self.connect((self.blocks_complex_to_imag_0, 0), (self.fft_filter_xxx_3, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_1, 0), (self.blocks_add_xx_0_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_complex_to_imag_0, 0))
        self.connect((self.blocks_multiply_xx_2, 0), (self.fft_filter_xxx_2, 0))
        self.connect((self.fft_filter_xxx_1, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.fft_filter_xxx_1, 0), (self.blocks_add_xx_0_0, 1))
        self.connect((self.fft_filter_xxx_2, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.fft_filter_xxx_2, 0), (self.blocks_multiply_const_vxx_0_1, 0))
        self.connect((self.fft_filter_xxx_3, 0), (self.blocks_multiply_xx_2, 1))
        self.connect((self.fir_filter_xxx_1, 0), (self.analog_pll_refout_cc_0, 0))
        self.connect((self, 0), (self.analog_quadrature_demod_cf_0, 0))

