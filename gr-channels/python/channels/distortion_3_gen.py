#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Third Order Distortion
# Author: mettus
# Generated: Thu Aug  1 12:37:59 2013
##################################################

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
import math

class distortion_3_gen(gr.hier_block2):

    def __init__(self, beta=0):
        gr.hier_block2.__init__(
            self, "Third Order Distortion",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.beta = beta

        ##################################################
        # Blocks
        ##################################################
        self.blocks_null_source_0 = blocks.null_source(gr.sizeof_float*1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((beta, ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.blocks_add_xx_0 = blocks.add_vcc(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_null_source_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self, 0), (self.blocks_complex_to_mag_squared_0, 0))
        self.connect((self, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self, 0))


# QT sink close method reimplementation

    def get_beta(self):
        return self.beta

    def set_beta(self, beta):
        self.beta = beta
        self.blocks_multiply_const_vxx_0.set_k((self.beta, ))


