#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2007-2020 Free Software Foundation, Inc.
#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: FM stereo demod block

import math

from gnuradio import gr
from gnuradio import blocks
from gnuradio import filter, fft
from gnuradio.filter import firdes
from gnuradio import analog


class wfm_rcv_pll(gr.hier_block2):
    def __init__(self, demod_rate, audio_decimation, deemph_tau):
        """
        Hierarchical block for demodulating a broadcast FM signal.

        The input is the downconverted complex baseband signal (gr_complex).
        The output is two streams of the demodulated audio (float) 0=Left, 1=Right.

        Args:
            demod_rate: input sample rate of complex baseband input. (float)
            audio_decimation: how much to decimate demod_rate to get to audio. (integer)
            deemph_tau: deemphasis ime constant in seconds (75us in US, 50us in EUR). (float)
        """
        gr.hier_block2.__init__(self, "wfm_rcv_pll",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(2, 2, gr.sizeof_float))      # Output signature

        if audio_decimation != int(audio_decimation):
            raise ValueError("audio_decimation needs to be an integer")
        audio_decimation = int(audio_decimation)

        ##################################################
        # Variables
        ##################################################
        self.demod_rate = demod_rate
        self.deemph_tau = deemph_tau
        self.stereo_carrier_filter_coeffs = stereo_carrier_filter_coeffs = firdes.band_pass(
            -2.0, demod_rate, 37600, 38400, 400, fft.window.WIN_HAMMING, 6.76)
        self.pilot_carrier_filter_coeffs = pilot_carrier_filter_coeffs = firdes.complex_band_pass(
            1.0, demod_rate, 18980, 19020, 1500, fft.window.WIN_HAMMING, 6.76)
        self.deviation = deviation = 75000
        self.audio_filter_coeffs = audio_filter_coeffs = firdes.low_pass(
            1, demod_rate, 15000, 1500, fft.window.WIN_HAMMING, 6.76)
        self.audio_decim = audio_decim = audio_decimation
        self.audio_rate = audio_rate = demod_rate / audio_decim
        self.samp_delay = samp_delay = (len(
            pilot_carrier_filter_coeffs) - 1) // 2 + (len(stereo_carrier_filter_coeffs) - 1) // 2

        ##################################################
        # Blocks
        ##################################################
        self.pilot_carrier_bpf = filter.fir_filter_fcc(
            1, pilot_carrier_filter_coeffs)
        self.pilot_carrier_bpf.declare_sample_delay(0)
        self.stereo_carrier_bpf = filter.fft_filter_fff(
            1, stereo_carrier_filter_coeffs, 1)
        self.stereo_carrier_bpf.declare_sample_delay(0)
        self.stereo_audio_lpf = filter.fft_filter_fff(
            audio_decim, audio_filter_coeffs, 1)
        self.stereo_audio_lpf.declare_sample_delay(0)
        self.mono_audio_lpf = filter.fft_filter_fff(
            audio_decim, audio_filter_coeffs, 1)
        self.mono_audio_lpf.declare_sample_delay(0)
        self.blocks_stereo_multiply = blocks.multiply_ff(1)
        self.blocks_pilot_multiply = blocks.multiply_cc(1)
        self.blocks_complex_to_imag = blocks.complex_to_imag(1)
        self.blocks_right_sub = blocks.sub_ff(1)
        self.blocks_left_add = blocks.add_ff(1)
        self.analog_quadrature_demod_cf = analog.quadrature_demod_cf(
            demod_rate / (2 * math.pi * deviation))
        self.analog_pll_refout_cc = analog.pll_refout_cc(
            0.001, 2 * math.pi * 19200 / demod_rate, 2 * math.pi * 18800 / demod_rate)
        self.analog_right_fm_deemph = analog.fm_deemph(
            fs=audio_rate, tau=deemph_tau)
        self.analog_left_fm_deemph = analog.fm_deemph(
            fs=audio_rate, tau=deemph_tau)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_float * 1, samp_delay)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_left_fm_deemph, 0), (self, 0))
        self.connect((self.analog_right_fm_deemph, 0), (self, 1))
        self.connect((self.analog_pll_refout_cc, 0),
                     (self.blocks_pilot_multiply, 1))
        self.connect((self.analog_pll_refout_cc, 0),
                     (self.blocks_pilot_multiply, 0))
        self.connect((self.analog_quadrature_demod_cf, 0),
                     (self.blocks_delay_0, 0))
        self.connect((self.blocks_delay_0, 0),
                     (self.blocks_stereo_multiply, 0))
        self.connect((self.blocks_delay_0, 0), (self.mono_audio_lpf, 0))
        self.connect((self.analog_quadrature_demod_cf, 0),
                     (self.pilot_carrier_bpf, 0))
        self.connect((self.blocks_left_add, 0),
                     (self.analog_left_fm_deemph, 0))
        self.connect((self.blocks_right_sub, 0),
                     (self.analog_right_fm_deemph, 0))
        self.connect((self.blocks_complex_to_imag, 0),
                     (self.stereo_carrier_bpf, 0))
        self.connect((self.blocks_pilot_multiply, 0),
                     (self.blocks_complex_to_imag, 0))
        self.connect((self.blocks_stereo_multiply, 0),
                     (self.stereo_audio_lpf, 0))  # L - R path
        self.connect((self.mono_audio_lpf, 0), (self.blocks_left_add, 1))
        self.connect((self.mono_audio_lpf, 0), (self.blocks_right_sub, 0))
        self.connect((self.stereo_audio_lpf, 0), (self.blocks_left_add, 0))
        self.connect((self.stereo_audio_lpf, 0), (self.blocks_right_sub, 1))
        self.connect((self.stereo_carrier_bpf, 0),
                     (self.blocks_stereo_multiply, 1))
        self.connect((self.pilot_carrier_bpf, 0),
                     (self.analog_pll_refout_cc, 0))
        self.connect((self, 0), (self.analog_quadrature_demod_cf, 0))
