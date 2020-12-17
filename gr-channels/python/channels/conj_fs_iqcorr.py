#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Conj FS IQBal
# Author: matt@ettus.com
# Description: Frequency Selective Conjugate Method IQ Balance Corrector
# Generated: Thu Aug  1 13:00:27 2013
##################################################

from gnuradio import blocks
from gnuradio import filter
from gnuradio import gr
from gnuradio.filter import firdes

class conj_fs_iqcorr(gr.hier_block2):

    def __init__(self, delay=0, taps=[]):
        gr.hier_block2.__init__(
            self, "Conj FS IQBal",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.delay = delay
        self.taps = taps

        ##################################################
        # Blocks
        ##################################################
        self.filter_fir_filter_xxx_0 = filter.fir_filter_ccc(1, (taps))
        self.delay_0 = blocks.delay(gr.sizeof_gr_complex*1, delay)
        self.blocks_conjugate_cc_0 = blocks.conjugate_cc()
        self.blocks_add_xx_0 = blocks.add_vcc(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_add_xx_0, 0), (self, 0))
        self.connect((self, 0), (self.blocks_conjugate_cc_0, 0))
        self.connect((self.filter_fir_filter_xxx_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_conjugate_cc_0, 0), (self.filter_fir_filter_xxx_0, 0))
        self.connect((self, 0), (self.delay_0, 0))
        self.connect((self.delay_0, 0), (self.blocks_add_xx_0, 0))


# QT sink close method reimplementation

    def get_delay(self):
        return self.delay

    def set_delay(self, delay):
        self.delay = delay
        self.delay_0.set_dly(self.delay)

    def get_taps(self):
        return self.taps

    def set_taps(self, taps):
        self.taps = taps
        self.filter_fir_filter_xxx_0.set_taps((self.taps))


