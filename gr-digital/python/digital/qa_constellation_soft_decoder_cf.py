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
from math import sqrt
from numpy import random, vectorize


class test_constellation_soft_decoder(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def helper_with_lut(self, prec, src_data, const_gen, const_sd_gen, decimals=5):
        cnst_pts, code = const_gen()
        Es = 1.0
        lut = digital.soft_dec_table(cnst_pts, code, prec, Es)

        constel = digital.const_normalization(cnst_pts, "POWER")
        maxamp = digital.min_max_axes(constel)

        expected_result = list()
        for s in src_data:
            res = digital.calc_soft_dec_from_table(s, lut, prec, maxamp)
            expected_result += res

        cnst = digital.constellation_calcdist(cnst_pts, code, 4, 1, digital.constellation.POWER_NORMALIZATION)
        cnst.set_soft_dec_lut(lut, int(prec))
        cnst.normalize(digital.constellation.POWER_NORMALIZATION)
        src = blocks.vector_source_c(src_data)
        op = digital.constellation_soft_decoder_cf(cnst.base())
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()  # fetch the contents of the sink
        # print "actual result", actual_result
        # print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, decimals)

    def helper_no_lut(self, prec, src_data, const_gen, const_sd_gen):
        cnst_pts, code = const_gen()
        cnst = digital.constellation_calcdist(cnst_pts, code, 2, 1)
        expected_result = list()
        for s in src_data:
            res = digital.calc_soft_dec(s, cnst.points(), code)
            expected_result += res

        src = blocks.vector_source_c(src_data)
        op = digital.constellation_soft_decoder_cf(cnst.base())
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()  # fetch the contents of the sink
        # print "actual result", actual_result
        # print "expected result", expected_result

        # Double vs. float precision issues between Python and C++, so
        # use only 4 decimals in comparisons.
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result, 4)

    def test_constellation_soft_decoder_cf_bpsk_3(self):
        prec = 3
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        self.helper_with_lut(
            prec,
            src_data,
            digital.psk_2_0x0,
            digital.sd_psk_2_0x0)

    def test_constellation_soft_decoder_cf_bpsk_8(self):
        prec = 8
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        self.helper_with_lut(
            prec,
            src_data,
            digital.psk_2_0x0,
            digital.sd_psk_2_0x0)

    def test_constellation_soft_decoder_cf_bpsk_8_rand(self):
        prec = 8
        src_data = vectorize(complex)(
            2 * random.randn(100), 2 * random.randn(100))
        self.helper_with_lut(
            prec,
            src_data,
            digital.psk_2_0x0,
            digital.sd_psk_2_0x0)

    def test_constellation_soft_decoder_cf_bpsk_8_rand2(self):
        prec = 8
        src_data = vectorize(complex)(
            2 * random.randn(100), 2 * random.randn(100))
        self.helper_no_lut(
            prec,
            src_data,
            digital.psk_2_0x0,
            digital.sd_psk_2_0x0)

    def test_constellation_soft_decoder_cf_qpsk_3(self):
        prec = 3
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        self.helper_with_lut(
            prec,
            src_data,
            digital.psk_4_0x0_0_1,
            digital.sd_psk_4_0x0_0_1)

    def test_constellation_soft_decoder_cf_qpsk_8(self):
        prec = 8
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        self.helper_with_lut(
            prec,
            src_data,
            digital.psk_4_0x0_0_1,
            digital.sd_psk_4_0x0_0_1)

    def test_constellation_soft_decoder_cf_qpsk_8_rand(self):
        prec = 8
        src_data = vectorize(complex)(
            2 * random.randn(100), 2 * random.randn(100))
        self.helper_with_lut(
            prec,
            src_data,
            digital.psk_4_0x0_0_1,
            digital.sd_psk_4_0x0_0_1,
            3)

    def test_constellation_soft_decoder_cf_qpsk_8_rand2(self):
        prec = 8
        src_data = vectorize(complex)(
            2 * random.randn(100), 2 * random.randn(100))
        self.helper_no_lut(
            prec,
            src_data,
            digital.psk_4_0x0_0_1,
            digital.sd_psk_4_0x0_0_1)

    def test_constellation_soft_decoder_cf_qam16_3(self):
        prec = 3
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        self.helper_with_lut(
            prec,
            src_data,
            digital.qam_16_0x0_0_1_2_3,
            digital.sd_qam_16_0x0_0_1_2_3)

    def test_constellation_soft_decoder_cf_qam16_8(self):
        prec = 8
        src_data = (-1.0 - 1.0j, 1.0 - 1.0j, -1.0 + 1.0j, 1.0 + 1.0j,
                    -2.0 - 2.0j, 2.0 - 2.0j, -2.0 + 2.0j, 2.0 + 2.0j,
                    -0.2 - 0.2j, 0.2 - 0.2j, -0.2 + 0.2j, 0.2 + 0.2j,
                    0.3 + 0.4j, 0.1 - 1.2j, -0.8 - 0.1j, -0.4 + 0.8j,
                    0.8 + 1.0j, -0.5 + 0.1j, 0.1 + 1.2j, -1.7 - 0.9j)
        self.helper_with_lut(
            prec,
            src_data,
            digital.qam_16_0x0_0_1_2_3,
            digital.sd_qam_16_0x0_0_1_2_3)

    def test_constellation_soft_decoder_cf_qam16_8_rand(self):
        prec = 8
        src_data = vectorize(complex)(
            2 * random.randn(100), 2 * random.randn(100))
        self.helper_with_lut(
            prec,
            src_data,
            digital.qam_16_0x0_0_1_2_3,
            digital.sd_qam_16_0x0_0_1_2_3,
            3)

    def test_constellation_soft_decoder_cf_qam16_8_rand2(self):
        prec = 8
        #src_data = vectorize(complex)(2*random.randn(100), 2*random.randn(100))
        src_data = vectorize(complex)(2 * random.randn(2), 2 * random.randn(2))
        self.helper_no_lut(
            prec,
            src_data,
            digital.qam_16_0x0_0_1_2_3,
            digital.sd_qam_16_0x0_0_1_2_3)


if __name__ == '__main__':
    gr_unittest.run(test_constellation_soft_decoder)
