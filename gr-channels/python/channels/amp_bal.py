#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Amplitude Balance
# Author: mettus
# Description: Restores IQ amplitude balance
# Generated: Thu Aug  1 11:47:46 2013
##################################################

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes

class amp_bal(gr.hier_block2):

    def __init__(self, alpha=0):
        gr.hier_block2.__init__(
            self, "Amplitude Balance",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.alpha = alpha

        ##################################################
        # Blocks
        ##################################################
        self.blocks_rms_xx0 = blocks.rms_ff(alpha)
        self.blocks_rms_xx = blocks.rms_ff(alpha)
        self.blocks_multiply_vxx1 = blocks.multiply_vff(1)
        self.blocks_float_to_complex = blocks.float_to_complex(1)
        self.blocks_divide_xx = blocks.divide_ff(1)
        self.blocks_complex_to_float = blocks.complex_to_float(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_float_to_complex, 0), (self, 0))
        self.connect((self, 0), (self.blocks_complex_to_float, 0))
        self.connect((self.blocks_complex_to_float, 0), (self.blocks_rms_xx, 0))
        self.connect((self.blocks_complex_to_float, 1), (self.blocks_rms_xx0, 0))
        self.connect((self.blocks_rms_xx, 0), (self.blocks_divide_xx, 0))
        self.connect((self.blocks_rms_xx0, 0), (self.blocks_divide_xx, 1))
        self.connect((self.blocks_complex_to_float, 0), (self.blocks_float_to_complex, 0))
        self.connect((self.blocks_complex_to_float, 1), (self.blocks_multiply_vxx1, 1))
        self.connect((self.blocks_divide_xx, 0), (self.blocks_multiply_vxx1, 0))
        self.connect((self.blocks_multiply_vxx1, 0), (self.blocks_float_to_complex, 1))


# QT sink close method reimplementation

    def get_alpha(self):
        return self.alpha

    def set_alpha(self, alpha):
        self.alpha = alpha
        self.blocks_rms_xx.set_alpha(self.alpha)
        self.blocks_rms_xx0.set_alpha(self.alpha)


