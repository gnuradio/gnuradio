#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math

from gnuradio import gr, gr_unittest, analog, blocks


def avg_mag_sqrd_c(x, alpha):
    y = [0, ]
    for xi in x:
        tmp = alpha * (xi.real * xi.real + xi.imag *
                       xi.imag) + (1 - alpha) * y[-1]
        y.append(tmp)
    return y


def avg_mag_sqrd_f(x, alpha):
    y = [0, ]
    for xi in x:
        tmp = alpha * (xi * xi) + (1 - alpha) * y[-1]
        y.append(tmp)
    return y


class test_probe_avg_mag_sqrd(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_c_001(self):
        alpha = 0.0001
        src_data = [
            1.0 + 1.0j,
            2.0 + 2.0j,
            3.0 + 3.0j,
            4.0 + 4.0j,
            5.0 + 5.0j,
            6.0 + 6.0j,
            7.0 + 7.0j,
            8.0 + 8.0j,
            9.0 + 9.0j,
            10.0 + 10.0j]
        expected_result = avg_mag_sqrd_c(src_data, alpha)[-1]

        src = blocks.vector_source_c(src_data)
        op = analog.probe_avg_mag_sqrd_c(0, alpha)

        self.tb.connect(src, op)
        self.tb.run()

        result_data = op.level()
        self.assertAlmostEqual(expected_result, result_data, 5)

    def test_cf_002(self):
        alpha = 0.0001
        src_data = [
            1.0 + 1.0j,
            2.0 + 2.0j,
            3.0 + 3.0j,
            4.0 + 4.0j,
            5.0 + 5.0j,
            6.0 + 6.0j,
            7.0 + 7.0j,
            8.0 + 8.0j,
            9.0 + 9.0j,
            10.0 + 10.0j]
        expected_result = avg_mag_sqrd_c(src_data, alpha)[0:-1]

        src = blocks.vector_source_c(src_data)
        op = analog.probe_avg_mag_sqrd_cf(0, alpha)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

    def test_f_003(self):
        alpha = 0.0001
        src_data = [1.0, 2.0, 3.0, 4.0, 5.0,
                    6.0, 7.0, 8.0, 9.0, 10.0]
        expected_result = avg_mag_sqrd_f(src_data, alpha)[-1]

        src = blocks.vector_source_f(src_data)
        op = analog.probe_avg_mag_sqrd_f(0, alpha)

        self.tb.connect(src, op)
        self.tb.run()

        result_data = op.level()
        self.assertAlmostEqual(expected_result, result_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_probe_avg_mag_sqrd)
