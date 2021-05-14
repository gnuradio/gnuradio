#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Radio Impairments Model
# Author: mettus
# Generated: Thu Aug  1 12:46:10 2013
##################################################

import math
from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes

#Import locally
from .phase_noise_gen import phase_noise_gen
from .iqbal_gen import iqbal_gen
from .distortion_2_gen import distortion_2_gen
from .distortion_3_gen import distortion_3_gen

class impairments(gr.hier_block2):

    def __init__(self,
                 phase_noise_mag=0,
                 magbal=0,
                 phasebal=0,
                 q_ofs=0,
                 i_ofs=0,
                 freq_offset=0,
                 gamma=0,
                 beta=0):
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
        self.phase_noise = phase_noise_gen(10.0**(phase_noise_mag / 20.0), .01)
        self.iq_imbalance = iqbal_gen(magbal, phasebal)
        self.channels_distortion_3_gen_0 = distortion_3_gen(beta)
        self.channels_distortion_2_gen_0 = distortion_2_gen(gamma)
        self.freq_modulator = blocks.multiply_cc()
        self.freq_offset_gen = analog.sig_source_c(1.0, analog.GR_COS_WAVE, freq_offset, 1, 0)
        self.freq_modulator_dcoffs = blocks.multiply_cc()
        self.freq_offset_conj = blocks.conjugate_cc()
        self.dc_offset = blocks.add_const_vcc((i_ofs + q_ofs* 1j, ))

        ##################################################
        # Frequency offset
        self.connect((self, 0), (self.freq_modulator, 1))
        self.connect((self.freq_offset_gen, 0), (self.freq_offset_conj, 0))
        self.connect((self.freq_offset_conj, 0), (self.freq_modulator, 0))
        # Most distortions can be strung in a row
        self.connect(
            (self.freq_modulator, 0),
            (self.phase_noise, 0),
            (self.channels_distortion_3_gen_0, 0),
            (self.channels_distortion_2_gen_0, 0),
            (self.iq_imbalance, 0),
            (self.dc_offset, 0),
        )
        # Frequency offset again
        self.connect((self.freq_offset_gen, 0), (self.freq_modulator_dcoffs, 0))
        self.connect((self.dc_offset, 0), (self.freq_modulator_dcoffs, 1))
        self.connect((self.freq_modulator_dcoffs, 0), (self, 0))


# QT sink close method reimplementation

    def get_phase_noise_mag(self):
        return self.phase_noise_mag

    def set_phase_noise_mag(self, phase_noise_mag):
        self.phase_noise_mag = phase_noise_mag
        self.phase_noise.set_noise_mag(10**(self.phase_noise_mag / 20.0))

    def get_magbal(self):
        return self.magbal

    def set_magbal(self, magbal):
        self.magbal = magbal
        self.iq_imbalance.set_magnitude(self.magbal)

    def get_phasebal(self):
        return self.phasebal

    def set_phasebal(self, phasebal):
        self.phasebal = phasebal
        self.iq_imbalance.set_phase(self.phasebal)

    def get_q_ofs(self):
        return self.q_ofs

    def set_q_ofs(self, q_ofs):
        self.q_ofs = q_ofs
        self.dc_offset.set_k((self.i_ofs + self.q_ofs* 1j, ))

    def get_i_ofs(self):
        return self.i_ofs

    def set_i_ofs(self, i_ofs):
        """Set inphase part of DC offset"""
        self.i_ofs = i_ofs
        self.dc_offset.set_k((self.i_ofs + self.q_ofs* 1j, ))

    def get_freq_offset(self):
        """Return frequency offset (normalized to 1.0)"""
        return self.freq_offset

    def set_freq_offset(self, freq_offset):
        """Set frequency offset (normalized to 1.0)"""
        self.freq_offset = freq_offset
        self.freq_offset_gen.set_frequency(self.freq_offset)

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
