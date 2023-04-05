#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 Ettus Research, a National Instruments Brand
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

import sip

from gnuradio import gr
from gnuradio import qtgui

from PyQt5 import QtGui
from PyQt5.QtWidgets import QWidget


class FosphorGlSink(gr.hier_block2):
    """
    Combined fosphor display + fosphor formatter
    """
    fft_size = 256
    num_bins = 64

    def __init__(self,
                 input_decim,
                 wf_decim,
                 wf_lines,
                 histo_decim,
                 scale,
                 alpha,
                 epsilon,
                 trise,
                 tdecay):

        gr.hier_block2.__init__(self,
                                "FosphorGlSink",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(0, 0, 0))

        self.formatter = qtgui.fosphor_formatter(
            self.fft_size, self.num_bins, input_decim, wf_decim,
            histo_decim, scale, alpha, epsilon, trise, tdecay)

        self.fosphor_sink = qtgui.fosphor_display(
            self.fft_size, self.num_bins, wf_lines)

        self.connect(self, self.formatter)
        self.connect((self.formatter, 0), (self.fosphor_sink, 0))
        self.connect((self.formatter, 1), (self.fosphor_sink, 1))

    def set_frequency_range(self, center_freq, samp_rate):
        """
        Convenience forwarding function
        """
        self.fosphor_sink.set_frequency_range(center_freq, samp_rate)

    def getWidget(self):
        return sip.wrapinstance(self.fosphor_sink.qwidget(), QWidget)
