#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import math

from gnuradio import gr, gr_unittest, analog, blocks


def sincos(x):
    return math.cos(x) + math.sin(x) * 1j


class test_frequency_modulator(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_fm_001(self):
        pi = math.pi
        sensitivity = pi / 4
        src_data = (1.0 / 4, 1.0 / 2, 1.0 / 4, -1.0 / 4, -1.0 / 2, -1 / 4.0)
        running_sum = (pi / 16, 3 * pi / 16, pi / 4, 3 * pi / 16, pi / 16, 0)
        expected_result = tuple([sincos(x) for x in running_sum])
        src = blocks.vector_source_f(src_data)
        op = analog.frequency_modulator_fc(sensitivity)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_frequency_modulator)
