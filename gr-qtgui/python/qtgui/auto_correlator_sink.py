#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import math
import sip

from gnuradio import gr
from gnuradio import qtgui
from gnuradio import blocks, fft, filter

from PyQt5 import QtGui
from PyQt5.QtWidgets import QWidget


class Normalize(gr.hier_block2):
    def __init__(self, vecsize=1024):
        gr.hier_block2.__init__(
            self, "Normalize",
            gr.io_signature(1, 1, gr.sizeof_float * vecsize),
            gr.io_signature(1, 1, gr.sizeof_float * vecsize),
        )

        ##################################################
        # Parameters
        ##################################################
        self.vecsize = vecsize

        ##################################################
        # Blocks
        ##################################################
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(
            gr.sizeof_float, vecsize)
        self.blocks_repeat_0 = blocks.repeat(gr.sizeof_float, vecsize)
        self.blocks_max_xx_0 = blocks.max_ff(vecsize)
        self.blocks_divide_xx_0 = blocks.divide_ff(vecsize)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_divide_xx_0, 0), (self, 0))
        self.connect((self.blocks_stream_to_vector_0, 0),
                     (self.blocks_divide_xx_0, 1))
        self.connect((self, 0), (self.blocks_max_xx_0, 0))
        self.connect((self.blocks_repeat_0, 0),
                     (self.blocks_stream_to_vector_0, 0))
        self.connect((self.blocks_max_xx_0, 0), (self.blocks_repeat_0, 0))
        self.connect((self, 0), (self.blocks_divide_xx_0, 0))

    def get_vecsize(self):
        return self.vecsize

    def set_vecsize(self, vecsize):
        self.vecsize = vecsize


class AutoCorrelator(gr.hier_block2):
    """
    This block uses the Wiener Khinchin theorem that the FFT of a signal's
    power spectrum is its auto-correlation function.
    FAC Size controls the FFT size and therefore the length of time
    (samp_rate/fac_size) the auto-correlation runs over.
    """

    def __init__(self, sample_rate, fac_size, fac_decimation, use_db):
        gr.hier_block2.__init__(self, "AutoCorrelator",
                                gr.io_signature(
                                    1, 1, gr.sizeof_gr_complex),  # Input sig
                                gr.io_signature(1, 1, gr.sizeof_float * fac_size))  # Output sig

        self.fac_size = fac_size
        self.fac_decimation = fac_decimation
        self.sample_rate = sample_rate

        streamToVec = blocks.stream_to_vector(
            gr.sizeof_gr_complex, self.fac_size)
        # Make sure N is at least 1
        decimation = int(self.sample_rate /
                         self.fac_size / self.fac_decimation)
        self.one_in_n = blocks.keep_one_in_n(
            gr.sizeof_gr_complex * self.fac_size, max(1, decimation))

        # FFT Note: No windowing.
        fac = fft.fft_vcc(self.fac_size, True, ())

        complex2Mag = blocks.complex_to_mag(self.fac_size)
        self.avg = filter.single_pole_iir_filter_ff(1.0, self.fac_size)

        fac_fac = fft.fft_vfc(self.fac_size, True, ())
        fac_c2mag = blocks.complex_to_mag(fac_size)

        # There's a note in Baz's block about needing to add 3 dB to each bin but the DC bin, however it was never implemented
        n = 20
        k = -20 * math.log10(self.fac_size)
        log = blocks.nlog10_ff(n, self.fac_size, k)

        if use_db:
            self.connect(self, streamToVec, self.one_in_n, fac,
                         complex2Mag, fac_fac, fac_c2mag, self.avg, log, self)
        else:
            self.connect(self, streamToVec, self.one_in_n, fac,
                         complex2Mag, fac_fac, fac_c2mag, self.avg, self)


class AutoCorrelatorSink(gr.hier_block2):
    """
    docstring for block AutoCorrelatorSink
    """

    def __init__(self, sample_rate, fac_size, fac_decimation, title, autoScale, grid, yMin, yMax, use_db):
        gr.hier_block2.__init__(self,
                                "AutoCorrelatorSink",
                                # Input signature
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(0, 0, 0))  # Output signature

        self.fac_size = fac_size
        self.fac_decimation = fac_decimation
        self.sample_rate = sample_rate

        autoCorr = AutoCorrelator(
            sample_rate, fac_size, fac_decimation, use_db)
        vecToStream = blocks.vector_to_stream(gr.sizeof_float, self.fac_size)

        self.timeSink = qtgui.time_sink_f(
            self.fac_size // 2, sample_rate, title, 1, None)
        self.timeSink.enable_grid(grid)
        self.timeSink.set_y_axis(yMin, yMax)
        self.timeSink.enable_autoscale(autoScale)
        self.timeSink.disable_legend()
        self.timeSink.set_update_time(0.1)

        if use_db:
            self.connect(self, autoCorr, vecToStream, self.timeSink)
        else:
            norm = Normalize(self.fac_size)
            self.connect(self, autoCorr, norm, vecToStream, self.timeSink)

    def getWidget(self):
        return sip.wrapinstance(self.timeSink.qwidget(), QWidget)
