#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Phase Noise Generator
# Author: mettus
# Generated: Thu Aug  1 11:59:39 2013
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import filter
from gnuradio import gr
from gnuradio.filter import firdes

class phase_noise_gen(gr.hier_block2):

    def __init__(self, noise_mag=0, alpha=0.1):
        gr.hier_block2.__init__(
            self, "Phase Noise Generator",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.noise_mag = noise_mag
        self.alpha = alpha

        ##################################################
        # Blocks
        ##################################################
        self.filter_single_pole_iir_filter_xx_0 = filter.single_pole_iir_filter_ff(alpha, 1)
        self.blocks_transcendental_0_0 = blocks.transcendental("sin", "float")
        self.blocks_transcendental_0 = blocks.transcendental("cos", "float")
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_float_to_complex_0 = blocks.float_to_complex(1)
        self.analog_noise_source_x_0 = analog.noise_source_f(analog.GR_GAUSSIAN, noise_mag, 42)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_float_to_complex_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_noise_source_x_0, 0), (self.filter_single_pole_iir_filter_xx_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self, 0))
        self.connect((self, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.filter_single_pole_iir_filter_xx_0, 0), (self.blocks_transcendental_0, 0))
        self.connect((self.filter_single_pole_iir_filter_xx_0, 0), (self.blocks_transcendental_0_0, 0))
        self.connect((self.blocks_transcendental_0, 0), (self.blocks_float_to_complex_0, 0))
        self.connect((self.blocks_transcendental_0_0, 0), (self.blocks_float_to_complex_0, 1))


# QT sink close method reimplementation

    def get_noise_mag(self):
        return self.noise_mag

    def set_noise_mag(self, noise_mag):
        self.noise_mag = noise_mag
        self.analog_noise_source_x_0.set_amplitude(self.noise_mag)

    def get_alpha(self):
        return self.alpha

    def set_alpha(self, alpha):
        self.alpha = alpha
        self.filter_single_pole_iir_filter_xx_0.set_taps(self.alpha)


