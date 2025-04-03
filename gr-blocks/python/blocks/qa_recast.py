#!/usr/bin/env python
#
# Copyright 2025 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
from numpy import pi, e, sqrt, array, arange, int16, int32, float32, uint8


class test_recast(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_char_to_float(self):
        expected_result = array([0, 1, sqrt(2), e, pi], dtype=float32)
        src_data = list(expected_result.view(uint8))
        expected_result = list(expected_result)

        src = blocks.vector_source_b(src_data)
        op = blocks.recast(gr.sizeof_char, gr.sizeof_float)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_short_to_float(self):
        expected_result = array([0, 1, sqrt(2), e, pi], dtype=float32)
        src_data = list(expected_result.view(int16))
        expected_result = list(expected_result)

        src = blocks.vector_source_s(src_data)
        op = blocks.recast(gr.sizeof_short, gr.sizeof_float)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_int_to_float(self):
        expected_result = array([0, 1, sqrt(2), e, pi], dtype=float32)
        src_data = list(expected_result.view(int32))
        expected_result = list(expected_result)

        src = blocks.vector_source_i(src_data)
        op = blocks.recast(gr.sizeof_int, gr.sizeof_float)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_char_to_short(self):
        expected_result = arange(-8, 8, dtype=int16)
        src_data = list(expected_result.view(uint8))
        expected_result = list(expected_result)

        src = blocks.vector_source_b(src_data)
        op = blocks.recast(gr.sizeof_char, gr.sizeof_short)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_short_to_int(self):
        expected_result = arange(-8, 8, dtype=int32)
        src_data = list(expected_result.view(int16))
        expected_result = list(expected_result)

        src = blocks.vector_source_s(src_data)
        op = blocks.recast(gr.sizeof_short, gr.sizeof_int)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_char_to_int(self):
        expected_result = arange(-16, 16, dtype=int32)
        src_data = list(expected_result.view(uint8))
        expected_result = list(expected_result)

        src = blocks.vector_source_b(src_data)
        op = blocks.recast(gr.sizeof_char, gr.sizeof_int)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_int_to_char(self):
        src_data = arange(-16, 16, dtype=int32)
        expected_result = list(src_data.view(uint8))
        src_data = list(src_data)

        src = blocks.vector_source_i(src_data)
        op = blocks.recast(gr.sizeof_int, gr.sizeof_char)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_int_to_short(self):
        src_data = arange(-16, 16, dtype=int32)
        expected_result = list(src_data.view(int16))
        src_data = list(src_data)

        src = blocks.vector_source_i(src_data)
        op = blocks.recast(gr.sizeof_int, gr.sizeof_short)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_short_to_char(self):
        src_data = arange(-16, 16, dtype=int16)
        expected_result = list(src_data.view(uint8))
        src_data = list(src_data)

        src = blocks.vector_source_s(src_data)
        op = blocks.recast(gr.sizeof_short, gr.sizeof_char)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)


if __name__ == "__main__":
    gr_unittest.run(test_recast)
