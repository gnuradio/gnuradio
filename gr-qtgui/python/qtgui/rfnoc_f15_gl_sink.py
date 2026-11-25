#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2023 Ettus Research, a National Instruments Brand
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from PyQt6 import sip

from gnuradio import gr
from gnuradio import qtgui

from PyQt6.QtWidgets import QWidget


class RfnocF15GlSink(gr.hier_block2):
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
                                "RfnocF15GlSink",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(0, 0, 0))

        self.formatter = qtgui.rfnoc_f15_formatter(
            self.fft_size, self.num_bins, input_decim, wf_decim,
            histo_decim, scale, alpha, epsilon, trise, tdecay)

        self.display = qtgui.rfnoc_f15_display(
            self.fft_size, self.num_bins, wf_lines)

        self.connect(self, self.formatter)
        self.connect((self.formatter, 0), (self.display, 0))
        self.connect((self.formatter, 1), (self.display, 1))

    def set_frequency_range(self, center_freq, samp_rate):
        """
        Convenience forwarding function
        """
        self.display.set_frequency_range(center_freq, samp_rate)

    def getWidget(self):
        return sip.wrapinstance(self.display.qwidget(), QWidget)
