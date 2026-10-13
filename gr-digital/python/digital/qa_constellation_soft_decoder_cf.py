#!/usr/bin/env python
#
# Copyright 2013-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest, digital, blocks
from gnuradio.digital import soft_dec_table_generator
from gnuradio.digital import psk_constellations, qam_constellations
import numpy as np

NO_NORM = digital.constellation.NO_NORMALIZATION


class test_constellation_soft_decoder(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def run_test(self, constel, src_data, expected, places):
        self.src = blocks.vector_source_c(src_data)
        self.snk = blocks.vector_sink_f()
        self.soft_decoder = digital.constellation_soft_decoder_cf(constel)
        self.tb.connect(self.src, self.soft_decoder, self.snk)
        self.tb.run()
        result = self.snk.data()
        self.assertFloatTuplesAlmostEqual(result, expected, places)

    def flatten(self, list_of_lists):
        return [i for inner_list in list_of_lists for i in inner_list]

    def llr_bpsk(self, samp, npwr, Es=1.0):
        """Calculate log-likelihood ratio for BPSK signal"""
        noise_i = npwr / 2
        return [2.0 * np.sqrt(Es) * samp.real / noise_i]

    def lut_index_to_complex(self, re, im, precision, axis_limit):
        """Conversion of lut indices to the complex plane, for testing
        the lookup table at exact points"""
        step = 2 * axis_limit / (2**precision - 1)
        return [
            complex(-axis_limit + r * step, -axis_limit + i * step)
            for r, i in zip(re, im)
        ]

    def test_custom_qpsk(self):
        """
        Use custom qpsk constellation with symbol map that is not involution
        01|11
        -----
        00|10
        """
        points = [1 + 1j, -1 + 1j, -1 - 1j, 1 - 1j]
        # decode = [3, 1, 0, 2]
        symbol_map = [2, 1, 3, 0]
        # Input is constel points in reverse order
        inputs = [1 - 1j, -1 - 1j, -1 + 1j, 1 + 1j]
        # Expected output is the decoded chunks in reverse order
        expected = [2, 0, 1, 3]
        constel = digital.constellation_calcdist(points, symbol_map, 4, 1,
                                                 NO_NORM)
        src = blocks.vector_source_c(inputs)
        softdecoder = digital.constellation_soft_decoder_cf(constel)
        slicer = digital.binary_slicer_fb()
        pack2 = blocks.pack_k_bits_bb(2)
        snk = blocks.vector_sink_b()
        self.tb.connect(src, softdecoder, slicer, pack2, snk)
        self.tb.run()
        out = snk.data()
        self.assertListEqual(out, expected)

    def test_bpsk_no_lut(self):
        points, coding = digital.psk_2()
        bpsk = digital.constellation_calcdist(points, coding, 2, 1)
        npwr = 0.5
        bpsk.set_npwr(0.5)
        src_data = [-0.5 + 0.2j, -1 + 1j, -1.25 - 0.4j, 0.3 - 0.8j]
        expected = self.flatten([self.llr_bpsk(s, npwr) for s in src_data])
        self.run_test(bpsk, src_data, expected, 5)

    def test_bpsk_lut(self):
        points, coding = digital.psk_2()
        bpsk = digital.constellation_calcdist(points, coding, 2, 1)
        npwr = 2
        bpsk.set_npwr(npwr)
        prec = 8
        bpsk.gen_soft_dec_lut(prec)
        # use lut indices to test exact sample points in lut
        re = [0, 30, 60, 0, 150, 255, 180, 255]
        im = [0, 45, 145, 255, 190, 255, 12, 0]
        max_axis = digital.min_max_axes(bpsk.points())
        PADDING = 2
        axis_limit = PADDING * max_axis
        src_data = self.lut_index_to_complex(re, im, prec, axis_limit)
        expected = self.flatten([self.llr_bpsk(s, npwr) for s in src_data])
        self.run_test(bpsk, src_data, expected, 5)

    def test_bpsk_lut_external_sd(self):
        points, coding = digital.psk_2()
        bpsk = digital.constellation_calcdist(points, coding, 2, 1)
        prec = 8
        axlimit = 2
        lut = soft_dec_table_generator(digital.sd_psk_2, prec, axlimit)
        bpsk.set_soft_dec_lut(lut, prec, axlimit)

        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        expected = self.flatten([digital.sd_psk_2(s) for s in src_data])
        # relax number of places; the src_data do not map to
        # exact points in the lookup table
        self.run_test(bpsk, src_data, expected, 1)

    def test_bpsk_python_no_lut(self):
        """ show that python soft decision is same as cpp constel """
        points, coding = digital.psk_2()
        bpsk = digital.constellation_calcdist(points, coding, 2, 1)
        npwr = 1.5
        bpsk.set_npwr(1.5)
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        expected = self.flatten([digital.calc_soft_dec(s, points, coding, npwr)
                                 for s in src_data])
        self.run_test(bpsk, src_data, expected, 5)

    def test_bpsk_python_lut(self):
        """ show that python table lookup is same as cpp constel """
        points, coding = digital.psk_2()
        bpsk = digital.constellation_calcdist(points, coding, 2, 1)
        npwr = 1.2
        prec = 8
        lut = digital.soft_dec_table(points, coding, prec, npwr)
        PADDING = 2
        max_axis = digital.min_max_axes(points)
        axlimit = PADDING * max_axis
        bpsk.set_soft_dec_lut(lut, prec, axlimit)
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)

        expected = self.flatten(
            [
                digital.calc_soft_dec_from_table(s, lut, prec, axlimit)
                for s in src_data
            ]
        )
        self.run_test(bpsk, src_data, expected, 6)

    def llr_qpsk(self, samp, npwr, Es):
        """ Loglikelihood ratio for digital.psk_4 constellation """
        dist_from_axis = np.sqrt(Es) / np.sqrt(2)
        npwr_i = npwr_q = npwr / 2
        llr_bit1 = 2 * dist_from_axis * samp.imag / npwr_q
        llr_bit0 = 2 * dist_from_axis * samp.real / npwr_i
        return [llr_bit1, llr_bit0]

    def test_qpsk_no_lut(self):
        points, coding = digital.psk_4()
        qpsk = digital.constellation_calcdist(points, coding, 4, 1, NO_NORM)
        src_data = [-0.5 - 0.5j, -1 + 1j, -1.25 + 0.8j, 0.3 - 0.1j]
        npwr = 0.75
        qpsk.set_npwr(npwr)
        Es = max(np.abs(qpsk.points())) ** 2
        expected = self.flatten([self.llr_qpsk(x, npwr, Es) for x in src_data])
        self.run_test(qpsk, src_data, expected, 5)

    def test_qpsk_lut(self):
        points, coding = digital.psk_4()
        qpsk = digital.constellation_calcdist(points, coding, 4, 1, NO_NORM)
        npwr = 2
        qpsk.set_npwr(npwr)
        prec = 8
        qpsk.gen_soft_dec_lut(prec)
        # use lut indices to test exact sample points
        re = [0, 30, 60, 0, 150, 255, 180, 255]
        im = [0, 45, 145, 255, 190, 255, 12, 0]
        PADDING = 2
        max_axis = digital.min_max_axes(qpsk.points())
        axis_limit = PADDING * max_axis
        src_data = self.lut_index_to_complex(re, im, prec, axis_limit)
        Es = max(np.abs(qpsk.points())) ** 2
        expected = self.flatten([self.llr_qpsk(s, npwr, Es) for s in src_data])
        self.run_test(qpsk, src_data, expected, 5)

    def test_qpsk_lut_external_sd(self):
        points, coding = digital.psk_4()
        qpsk = digital.constellation_calcdist(points, coding, 4, 1, NO_NORM)
        prec = 8
        axlimit = 2
        lut = soft_dec_table_generator(digital.sd_psk_4, prec, axlimit)
        qpsk.set_soft_dec_lut(lut, prec, axlimit)
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        expected = self.flatten([digital.sd_psk_4(s) for s in src_data])
        self.run_test(qpsk, src_data, expected, 1)

    def llr_qam(self, samp, npwr):
        """ Log-likelihood ratio for constellation in digital.qam_16 """
        def gexp(x, mean, pwr):
            """calculate exponent portion of gaussian pdf"""
            return np.exp(-0.5 / pwr * (x - mean) ** 2)

        npwr_i = npwr_q = npwr / 2
        # the qam gray coding allows the first two bits to
        # depend only on the real part
        IP = 1 / 3  # QAM inner points
        OP = 1.0  # QAM outer points
        nom3 = gexp(samp.real, IP, npwr_i) + gexp(samp.real, OP, npwr_i)
        den3 = gexp(samp.real, -IP, npwr_i) + gexp(samp.real, -OP, npwr_i)
        llrbit3 = np.log(nom3 / den3)
        nom2 = gexp(samp.real, -IP, npwr_i) + gexp(samp.real, IP, npwr_i)
        den2 = gexp(samp.real, -OP, npwr_i) + gexp(samp.real, OP, npwr_i)
        llrbit2 = np.log(nom2 / den2)
        # similarly, the last two bits only depend on the imag part
        nom1 = gexp(samp.imag, IP, npwr_q) + gexp(samp.imag, OP, npwr_q)
        den1 = gexp(samp.imag, -IP, npwr_q) + gexp(samp.imag, -OP, npwr_q)
        llrbit1 = np.log(nom1 / den1)
        nom0 = gexp(samp.imag, IP, npwr_q) + gexp(samp.imag, -IP, npwr_q)
        den0 = gexp(samp.imag, OP, npwr_q) + gexp(samp.imag, -OP, npwr_q)
        llrbit0 = np.log(nom0 / den0)
        return [llrbit3, llrbit2, llrbit1, llrbit0]

    def test_qam_no_lut(self):
        points, coding = digital.qam_16()
        qam = digital.constellation_calcdist(points, coding, 4, 1, NO_NORM)
        npwr = 1.25
        qam.set_npwr(npwr)
        src_data = [-3 - 3j, -2 + 1j, -1.25 + 0.8j, 0.3 - 0.1j, 4 + 3j]
        expected = self.flatten([self.llr_qam(s, npwr) for s in src_data])
        self.run_test(qam, src_data, expected, 5)

    def test_qam_lut(self):
        points, coding = digital.qam_16()
        qam = digital.constellation_calcdist(points, coding, 4, 1, NO_NORM)
        npwr = 2.2
        qam.set_npwr(npwr)
        prec = 8
        qam.gen_soft_dec_lut(prec)
        # use lut indices to test exact sample points in lut
        re = [0, 30, 60, 0, 150, 255, 180, 255]
        im = [0, 45, 145, 255, 190, 255, 12, 0]
        max_axis = digital.min_max_axes(qam.points())
        PADDING = 2
        axis_limit = PADDING * max_axis
        src_data = self.lut_index_to_complex(re, im, prec, axis_limit)
        expected = self.flatten([self.llr_qam(s, npwr) for s in src_data])
        self.run_test(qam, src_data, expected, 5)

    def test_qam16_lut_external_sd(self):
        points, coding = digital.qam_16()
        qam = digital.constellation_calcdist(points, coding, 4, 1)
        axlimit = 2
        prec = 8
        lut = soft_dec_table_generator(digital.sd_qam_16, prec, axlimit)
        qam.set_soft_dec_lut(lut, prec, axlimit)

        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)

        expected = self.flatten([digital.sd_qam_16(s) for s in src_data])
        self.run_test(qam, src_data, expected, 1)


if __name__ == "__main__":
    gr_unittest.run(test_constellation_soft_decoder)
    #points, syms = digital.psk_2()
    #sd = digital.sd_psk_2
    #breakpoint()
