#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: IQ Imbalance Generator
# Author: mettus
# Generated: Thu Aug  1 12:08:07 2013
##################################################

from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
import math

class iqbal_gen(gr.hier_block2):

    def __init__(self, magnitude=0, phase=0):
        gr.hier_block2.__init__(
            self, "IQ Imbalance Generator",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.magnitude = magnitude
        self.phase = phase

        ##################################################
        # Blocks
        ##################################################
        self.mag = blocks.multiply_const_vff((math.pow(10,magnitude/20.0), ))
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vff((math.sin(phase*math.pi/180.0), ))
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.blocks_complex_to_float_0 = blocks.complex_to_float(1)
        self.blocks_add_xx_0 = blocks.add_vff(1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_float_to_complex_0, 0), (self, 0))
        self.connect((self, 0), (self.blocks_complex_to_float_0, 0))
        self.connect((self.blocks_complex_to_float_0, 0), (self.mag, 0))
        self.connect((self.mag, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_add_xx_0, 0), (self.blocks_float_to_complex_0, 1))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.blocks_complex_to_float_0, 1), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_complex_to_float_0, 0), (self.blocks_multiply_const_vxx_0, 0))


# QT sink close method reimplementation

    def get_magnitude(self):
        return self.magnitude

    def set_magnitude(self, magnitude):
        self.magnitude = magnitude
        self.mag.set_k((math.pow(10,self.magnitude/20.0), ))

    def get_phase(self):
        return self.phase

    def set_phase(self, phase):
        self.phase = phase
        self.blocks_multiply_const_vxx_0.set_k((math.sin(self.phase*math.pi/180.0), ))


