#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_threshold(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_01(self):
        tb = self.tb

        data = [0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2]

        expected_result = [0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1]

        src = blocks.vector_source_f(data, False)
        op = blocks.threshold_ff(1, 1)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertEqual(expected_result, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_threshold)
