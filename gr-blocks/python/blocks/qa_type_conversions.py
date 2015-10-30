#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest, blocks

from math import sqrt, atan2

class test_type_conversions(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_char_to_float_identity(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = blocks.vector_source_b(src_data)
        op = blocks.char_to_float()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_char_to_float_scale(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (0.5, 1.0, 1.5, 2.0, 2.5)
        src = blocks.vector_source_b(src_data)
        op = blocks.char_to_float(scale=2.0)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_char_to_short(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (256, 512, 768, 1024, 1280)
        src = blocks.vector_source_b(src_data)
        op = blocks.char_to_short()
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_complex_to_interleaved_char(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_interleaved_char()
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_complex_to_interleaved_short(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_interleaved_short()
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_complex_to_float_1(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1.0, 3.0, 5.0, 7.0, 9.0)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_float()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_complex_to_float_2(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data1 = (1.0, 3.0, 5.0, 7.0, 9.0)
        expected_data2 = (2.0, 4.0, 6.0, 8.0, 10.0)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_float()
        dst1 = blocks.vector_sink_f()
        dst2 = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect((op, 0), dst1)
        self.tb.connect((op, 1), dst2)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data1, dst1.data())
        self.assertFloatTuplesAlmostEqual(expected_data2, dst2.data())

    def test_complex_to_real(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1.0, 3.0, 5.0, 7.0, 9.0)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_real()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_complex_to_imag(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (2.0, 4.0, 6.0, 8.0, 10.0)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_imag()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_complex_to_mag(self):
        src_data = (1+2j, 3-4j, 5+6j, 7-8j, -9+10j)
        expected_data = (sqrt(5), sqrt(25), sqrt(61), sqrt(113), sqrt(181))
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_mag()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data(), 5)

    def test_complex_to_mag_squared(self):
        src_data = (1+2j, 3-4j, 5+6j, 7-8j, -9+10j)
        expected_data = (5.0, 25.0, 61.0, 113.0, 181.0)
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_mag_squared()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_complex_to_arg(self):
        src_data = (1+2j, 3-4j, 5+6j, 7-8j, -9+10j)
        expected_data = (atan2(2, 1), atan2(-4,3), atan2(6, 5), atan2(-8, 7), atan2(10,-9))
        src = blocks.vector_source_c(src_data)
        op = blocks.complex_to_arg()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data(), 2)

    def test_float_to_char_identity(self):
        src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        expected_data = (1, 2, 3, 4, 5)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_char()
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_float_to_char_scale(self):
        src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        expected_data = (5, 10, 15, 20, 25)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_char(1, 5)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_float_to_complex_1(self):
        src_data = (1.0, 3.0, 5.0, 7.0, 9.0)
        expected_data = (1+0j, 3+0j, 5+0j, 7+0j, 9+0j)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_complex()
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_float_to_complex_2(self):
        src1_data = (1.0, 3.0, 5.0, 7.0, 9.0)
        src2_data = (2.0, 4.0, 6.0, 8.0, 10.0)
        expected_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        src1 = blocks.vector_source_f(src1_data)
        src2 = blocks.vector_source_f(src2_data)
        op = blocks.float_to_complex()
        dst = blocks.vector_sink_c()
        self.tb.connect(src1, (op, 0))
        self.tb.connect(src2, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_float_to_int_identity(self):
        src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        expected_data = (1, 2, 3, 4, 5)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_int()
        dst = blocks.vector_sink_i()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_float_to_int_scale(self):
        src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        expected_data = (5, 10, 15, 20, 25)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_int(1, 5)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_float_to_short_identity(self):
        src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        expected_data = (1, 2, 3, 4, 5)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_short()
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_float_to_short_scale(self):
        src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        expected_data = (5, 10, 15, 20, 25)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_short(1, 5)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_float_to_uchar(self):
        src_data = (1.0, -2.0, 3.0, -4.0, 256.0)
        expected_data = (1, 0, 3, 0, 255)
        src = blocks.vector_source_f(src_data)
        op = blocks.float_to_uchar()
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_int_to_float_identity(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = blocks.vector_source_i(src_data)
        op = blocks.int_to_float()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_int_to_float_scale(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (0.2, 0.4, 0.6, 0.8, 1.0)
        src = blocks.vector_source_i(src_data)
        op = blocks.int_to_float(1, 5)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_interleaved_short_to_complex(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        expected_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        src = blocks.vector_source_s(src_data)
        op = blocks.interleaved_short_to_complex()
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_short_to_char(self):
        src_data = (256, 512, 768, 1024, 1280)
        expected_data = (1, 2, 3, 4, 5)
        src = blocks.vector_source_s(src_data)
        op = blocks.short_to_char()
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_short_to_float_identity(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = blocks.vector_source_s(src_data)
        op = blocks.short_to_float()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_short_to_float_scale(self):
        src_data = (5, 10, 15, 20, 25)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = blocks.vector_source_s(src_data)
        op = blocks.short_to_float(1, 5)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_uchar_to_float(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = blocks.vector_source_b(src_data)
        op = blocks.uchar_to_float()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())


if __name__ == '__main__':
    gr_unittest.run(test_type_conversions, "test_type_conversions.xml")
