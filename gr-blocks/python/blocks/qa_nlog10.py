#!/usr/bin/env python
#
# Copyright 2005,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_nlog10(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = (1, 10, 100, 1000, 10000, 100000)
        expected_result = (0, 10, 20, 30, 40, 50)
        src = blocks.vector_source_f(src_data)
        op = blocks.nlog10_ff(10)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_nlog10)
