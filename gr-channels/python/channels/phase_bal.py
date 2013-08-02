#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: IQ Phase Balancer
# Author: matt@ettus.com
# Generated: Thu Aug  1 11:49:41 2013
##################################################

from gnuradio import blocks
from gnuradio import filter
from gnuradio import gr
from gnuradio.filter import firdes

class phase_bal(gr.hier_block2):

    def __init__(self, alpha=0):
        gr.hier_block2.__init__(
            self, "IQ Phase Balancer",
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
        self.filter_single_pole_iir_filter_xx_0 = filter.single_pole_iir_filter_ff(alpha, 1)
        self.blocks_sub_xx_1 = blocks.sub_ff(1)
        self.blocks_sub_xx_0 = blocks.sub_ff(1)
        self.blocks_multiply_xx_2 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_1 = blocks.multiply_vff(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vff(1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff((2, ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_divide_xx_0 = blocks.divide_ff(1)
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.blocks_complex_to_float_0 = blocks.complex_to_float(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_complex_to_float_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_complex_to_float_0, 1), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_multiply_xx_0, 0), (self.blocks_divide_xx_0, 0))
        self.connect((self.blocks_sub_xx_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.blocks_multiply_xx_1, 0), (self.blocks_sub_xx_0, 1))
        self.connect((self.filter_single_pole_iir_filter_xx_0, 0), (self.blocks_multiply_xx_1, 1))
        self.connect((self.blocks_complex_to_float_0, 0), (self.blocks_multiply_xx_1, 0))
        self.connect((self.blocks_multiply_xx_2, 0), (self.blocks_sub_xx_1, 1))
        self.connect((self.blocks_complex_to_float_0, 1), (self.blocks_sub_xx_0, 0))
        self.connect((self.blocks_sub_xx_1, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.blocks_divide_xx_0, 1))
        self.connect((self.blocks_complex_to_float_0, 0), (self.blocks_sub_xx_1, 0))
        self.connect((self.blocks_divide_xx_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.filter_single_pole_iir_filter_xx_0, 0))
        self.connect((self, 0), (self.blocks_complex_to_float_0, 0))
        self.connect((self, 0), (self.blocks_complex_to_mag_squared_0, 0))
        self.connect((self.blocks_float_to_complex_0, 0), (self, 0))
        self.connect((self.filter_single_pole_iir_filter_xx_0, 0), (self.blocks_multiply_xx_2, 0))
        self.connect((self.blocks_complex_to_float_0, 1), (self.blocks_multiply_xx_2, 1))


# QT sink close method reimplementation

    def get_alpha(self):
        return self.alpha

    def set_alpha(self, alpha):
        self.alpha = alpha
        self.filter_single_pole_iir_filter_xx_0.set_taps(self.alpha)


