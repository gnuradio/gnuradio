#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Radio Impairments Model
# Author: mettus
# Generated: Thu Aug  1 12:46:10 2013
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
import math

#Import locally
from phase_noise_gen import *
from iqbal_gen import *
from distortion_2_gen import *
from distortion_3_gen import *

class impairments(gr.hier_block2):

    def __init__(self, phase_noise_mag=0, magbal=0, phasebal=0, q_ofs=0, i_ofs=0, freq_offset=0, gamma=0, beta=0):
        gr.hier_block2.__init__(
            self, "Radio Impairments Model",
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
            gr.io_signature(1, 1, gr.sizeof_gr_complex*1),
        )

        ##################################################
        # Parameters
        ##################################################
        self.phase_noise_mag = phase_noise_mag
        self.magbal = magbal
        self.phasebal = phasebal
        self.q_ofs = q_ofs
        self.i_ofs = i_ofs
        self.freq_offset = freq_offset
        self.gamma = gamma
        self.beta = beta

        ##################################################
        # Blocks
        ##################################################
        self.channels_phase_noise_gen_0_0 = phase_noise_gen(math.pow(10.0,phase_noise_mag/20.0), .01)
        self.channels_iqbal_gen_0 = iqbal_gen(magbal, phasebal)
        self.channels_distortion_3_gen_0 = distortion_3_gen(beta)
        self.channels_distortion_2_gen_0 = distortion_2_gen(gamma)
        self.blocks_multiply_xx_0_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_conjugate_cc_0 = blocks.conjugate_cc()
        self.blocks_add_const_vxx_0 = blocks.add_const_vcc((i_ofs + q_ofs* 1j, ))
        self.analog_sig_source_x_0 = analog.sig_source_c(1.0, analog.GR_COS_WAVE, freq_offset, 1, 0)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.channels_phase_noise_gen_0_0, 0), (self.channels_distortion_3_gen_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self, 0))
        self.connect((self.blocks_add_const_vxx_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.blocks_multiply_xx_0_0, 0), (self.channels_phase_noise_gen_0_0, 0))
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_conjugate_cc_0, 0))
        self.connect((self, 0), (self.blocks_multiply_xx_0_0, 1))
        self.connect((self.blocks_conjugate_cc_0, 0), (self.blocks_multiply_xx_0_0, 0))
        self.connect((self.channels_iqbal_gen_0, 0), (self.blocks_add_const_vxx_0, 0))
        self.connect((self.channels_distortion_3_gen_0, 0), (self.channels_distortion_2_gen_0, 0))
        self.connect((self.channels_distortion_2_gen_0, 0), (self.channels_iqbal_gen_0, 0))


# QT sink close method reimplementation

    def get_phase_noise_mag(self):
        return self.phase_noise_mag

    def set_phase_noise_mag(self, phase_noise_mag):
        self.phase_noise_mag = phase_noise_mag
        self.channels_phase_noise_gen_0_0.set_noise_mag(math.pow(10.0,self.phase_noise_mag/20.0))

    def get_magbal(self):
        return self.magbal

    def set_magbal(self, magbal):
        self.magbal = magbal
        self.channels_iqbal_gen_0.set_magnitude(self.magbal)

    def get_phasebal(self):
        return self.phasebal

    def set_phasebal(self, phasebal):
        self.phasebal = phasebal
        self.channels_iqbal_gen_0.set_phase(self.phasebal)

    def get_q_ofs(self):
        return self.q_ofs

    def set_q_ofs(self, q_ofs):
        self.q_ofs = q_ofs
        self.blocks_add_const_vxx_0.set_k((self.i_ofs + self.q_ofs* 1j, ))

    def get_i_ofs(self):
        return self.i_ofs

    def set_i_ofs(self, i_ofs):
        self.i_ofs = i_ofs
        self.blocks_add_const_vxx_0.set_k((self.i_ofs + self.q_ofs* 1j, ))

    def get_freq_offset(self):
        return self.freq_offset

    def set_freq_offset(self, freq_offset):
        self.freq_offset = freq_offset
        self.analog_sig_source_x_0.set_frequency(self.freq_offset)

    def get_gamma(self):
        return self.gamma

    def set_gamma(self, gamma):
        self.gamma = gamma
        self.channels_distortion_2_gen_0.set_beta(self.gamma)

    def get_beta(self):
        return self.beta

    def set_beta(self, beta):
        self.beta = beta
        self.channels_distortion_3_gen_0.set_beta(self.beta)


