#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import ctypes


class test_endian_swap(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [1, 2, 3, 4]
        expected_result = [256, 512, 768, 1024]

        src = blocks.vector_source_s(src_data)
        op = blocks.endian_swap(2)
        dst = blocks.vector_sink_s()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

    def test_002(self):

        src_data = [1, 2, 3, 4]
        expected_result = [16777216, 33554432, 50331648, 67108864]

        src = blocks.vector_source_i(src_data)
        op = blocks.endian_swap(4)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)


if __name__ == '__main__':
    gr_unittest.run(test_endian_swap)
