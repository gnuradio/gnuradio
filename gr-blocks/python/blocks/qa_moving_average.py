#!/usr/bin/env python
#
# Copyright 2013,2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

import math
import random


def make_random_complex_tuple(L, scale=1):
    result = []
    for x in range(L):
        result.append(scale * complex(2 * random.random() - 1,
                                      2 * random.random() - 1))
    return tuple(result)


def make_random_float_tuple(L, scale=1):
    result = []
    for x in range(L):
        result.append(scale * (2 * random.random() - 1))
    return tuple(result)


class test_moving_average(gr_unittest.TestCase):
    def assertListAlmostEqual(self, list1, list2, tol):
        self.assertEqual(len(list1), len(list2))
        for a, b in zip(list1, list2):
            self.assertAlmostEqual(a, b, tol)

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    # These tests will always pass and are therefore useless. 100 random numbers [-1,1) are
    # getting summed up and scaled with 0.001. Then, an assertion verifies a result near 0,
    # which is the case even if the block is malfunctioning.

    def test_01(self):
        tb = self.tb

        N = 10000
        data = make_random_float_tuple(N, 1)
        expected_result = N * [0, ]

        src = blocks.vector_source_f(data, False)
        op = blocks.moving_average_ff(100, 0.001)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        # make sure result is close to zero
        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 1)

    def test_02(self):
        tb = self.tb

        N = 10000
        data = make_random_complex_tuple(N, 1)
        expected_result = N * [0, ]

        src = blocks.vector_source_c(data, False)
        op = blocks.moving_average_cc(100, 0.001)
        dst = blocks.vector_sink_c()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        # make sure result is close to zero
        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 1)

    # This tests implement own moving average to verify correct behaviour of
    # the block

    def test_vector_int(self):
        tb = self.tb

        vlen = 5
        N = 10 * vlen
        data = make_random_float_tuple(N, 2**10)
        data = [int(d * 1000) for d in data]
        src = blocks.vector_source_i(data, False)
        one_to_many = blocks.stream_to_streams(gr.sizeof_int, vlen)
        one_to_vector = blocks.stream_to_vector(gr.sizeof_int, vlen)
        many_to_vector = blocks.streams_to_vector(gr.sizeof_int, vlen)
        isolated = [blocks.moving_average_ii(100, 1) for i in range(vlen)]
        dut = blocks.moving_average_ii(100, 1, vlen=vlen)
        dut_dst = blocks.vector_sink_i(vlen=vlen)
        ref_dst = blocks.vector_sink_i(vlen=vlen)

        tb.connect(src, one_to_many)
        tb.connect(src, one_to_vector, dut, dut_dst)
        tb.connect(many_to_vector, ref_dst)
        for idx, single in enumerate(isolated):
            tb.connect((one_to_many, idx), single, (many_to_vector, idx))

        tb.run()

        dut_data = dut_dst.data()
        ref_data = ref_dst.data()

        # make sure result is close to zero
        self.assertEqual(dut_data, ref_data)

    def test_vector_complex(self):
        tb = self.tb

        vlen = 5
        N = 10 * vlen
        data = make_random_complex_tuple(N, 2**10)
        src = blocks.vector_source_c(data, False)
        one_to_many = blocks.stream_to_streams(gr.sizeof_gr_complex, vlen)
        one_to_vector = blocks.stream_to_vector(gr.sizeof_gr_complex, vlen)
        many_to_vector = blocks.streams_to_vector(gr.sizeof_gr_complex, vlen)
        isolated = [blocks.moving_average_cc(100, 1) for i in range(vlen)]
        dut = blocks.moving_average_cc(100, 1, vlen=vlen)
        dut_dst = blocks.vector_sink_c(vlen=vlen)
        ref_dst = blocks.vector_sink_c(vlen=vlen)

        tb.connect(src, one_to_many)
        tb.connect(src, one_to_vector, dut, dut_dst)
        tb.connect(many_to_vector, ref_dst)
        for idx, single in enumerate(isolated):
            tb.connect((one_to_many, idx), single, (many_to_vector, idx))

        tb.run()

        dut_data = dut_dst.data()
        ref_data = ref_dst.data()

        # make sure result is close to zero
        self.assertListAlmostEqual(dut_data, ref_data, tol=3)

    def test_complex_scalar(self):
        tb = self.tb

        N = 10000  # number of samples
        history = 100  # num of samples to average
        data = make_random_complex_tuple(N, 1)  # generate random data

        #  pythonic MA filter
        data_padded = (history - 1) * \
            [complex(0.0, 0.0)] + list(data)  # history
        expected_result = []
        moving_sum = sum(data_padded[:history - 1])
        for i in range(N):
            moving_sum += data_padded[i + history - 1]
            expected_result.append(moving_sum)
            moving_sum -= data_padded[i]

        src = blocks.vector_source_c(data, False)
        op = blocks.moving_average_cc(history, 1)
        dst = blocks.vector_sink_c()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        # make sure result is close to zero
        self.assertListAlmostEqual(expected_result, dst_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_moving_average)
