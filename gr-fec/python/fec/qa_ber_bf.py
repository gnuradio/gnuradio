#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy
import copy

from gnuradio import gr, gr_unittest, blocks
from gnuradio import fec


class test_ber_bf(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        # Cause a single bit error out of 8*N bits
        # using streaming mode

        mode = False
        N = 10000
        data0 = numpy.random.randint(0, 256, N).tolist()
        data1 = copy.deepcopy(data0)
        data1[0] ^= 0x01

        src0 = blocks.vector_source_b(data0)
        src1 = blocks.vector_source_b(data1)
        op = fec.ber_bf(mode)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        data = dst.data()
        # [numpy.log10(1.0 / (8.0 * N)), ]
        expected_result = self.log_ber(1., N)

        self.assertFloatTuplesAlmostEqual(expected_result, data, 5)

    def test_001(self):
        # Cause a single bit error out of 8*N bits
        # using test mode

        mode = True
        N = 1000
        data0 = numpy.random.randint(0, 256, N).tolist()
        data1 = copy.deepcopy(data0)
        data1[0] ^= 0x01

        src0 = blocks.vector_source_b(data0)
        src1 = blocks.vector_source_b(data1)
        op = fec.ber_bf(mode, 1)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        data = dst.data()
        expected_result = self.log_ber(1., N)

        self.assertFloatTuplesAlmostEqual(expected_result, data, 5)

    def test_002(self):
        # Cause 8 bit errors out of 8*N bits
        # using test mode

        mode = True
        N = 1000
        data0 = numpy.random.randint(0, 256, N).tolist()
        data1 = copy.deepcopy(data0)
        data1[0] ^= 0xFF

        src0 = blocks.vector_source_b(data0)
        src1 = blocks.vector_source_b(data1)
        op = fec.ber_bf(mode, 1, -2.0)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        data = dst.data()
        expected_result = self.log_ber(8., N)

        self.assertFloatTuplesAlmostEqual(expected_result, data, 5)

    def test_003(self):
        # Cause a 8 bit errors out of 8*N bits
        # using test mode
        # Exit if BER < -2.0

        mode = True
        N = 1000
        data0 = numpy.random.randint(0, 256, N).tolist()
        data1 = copy.deepcopy(data0)
        data1[0] ^= 0xFF

        src0 = blocks.vector_source_b(data0)
        src1 = blocks.vector_source_b(data1)
        op = fec.ber_bf(mode, 10, -2.0)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        data = dst.data()
        expected_result = [-2.0, ]

        print(data)
        print(expected_result)

        self.assertFloatTuplesAlmostEqual(expected_result, data, 5)

    def test_004(self):
        # Cause 16 consecutive bit errors out of 8*N bits
        # make sure bytes are only read once.
        # using streaming mode

        mode = False
        N = 10000
        data0 = numpy.random.randint(0, 256, N).tolist()
        data1 = copy.deepcopy(data0)
        data1[0] ^= 0xFF
        data1[1] ^= 0xFF

        src0 = blocks.vector_source_b(data0)
        src1 = blocks.vector_source_b(data1)
        op = fec.ber_bf(mode)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()

        data = dst.data()
        expected_result = self.log_ber(16, N)

        self.assertFloatTuplesAlmostEqual(expected_result, data, 5)

    def log_ber(self, n_errors, N):
        return numpy.log10(1. * n_errors / (8.0 * N)),


if __name__ == '__main__':
    gr_unittest.run(test_ber_bf)
