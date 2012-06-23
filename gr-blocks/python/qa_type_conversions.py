#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
import blocks_swig

class test_type_conversions(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_char_to_float_identity(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = gr.vector_source_b(src_data)
        op = blocks_swig.char_to_float()
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_char_to_float_scale(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (0.5, 1.0, 1.5, 2.0, 2.5)
        src = gr.vector_source_b(src_data)
        op = blocks_swig.char_to_float(scale=2.0)
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_char_to_short(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (256, 512, 768, 1024, 1280)
        src = gr.vector_source_b(src_data)
        op = blocks_swig.char_to_short()
        dst = gr.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_complex_to_interleaved_short(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = gr.vector_source_c(src_data)
        op = blocks_swig.complex_to_interleaved_short()
        dst = gr.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_data, dst.data())

    def test_complex_to_float_1(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1.0, 3.0, 5.0, 7.0, 9.0)
        src = gr.vector_source_c(src_data)
        op = blocks_swig.complex_to_float()
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_complex_to_float_2(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data1 = (1.0, 3.0, 5.0, 7.0, 9.0)
        expected_data2 = (2.0, 4.0, 6.0, 8.0, 10.0)
        src = gr.vector_source_c(src_data)
        op = blocks_swig.complex_to_float()
        dst1 = gr.vector_sink_f()
        dst2 = gr.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect((op, 0), dst1)
        self.tb.connect((op, 1), dst2)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data1, dst1.data())
        self.assertFloatTuplesAlmostEqual(expected_data2, dst2.data())

    def test_complex_to_real(self):
        src_data = (1+2j, 3+4j, 5+6j, 7+8j, 9+10j)
        expected_data = (1.0, 3.0, 5.0, 7.0, 9.0)
        src = gr.vector_source_c(src_data)
        op = blocks_swig.complex_to_real()
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())


if __name__ == '__main__':
    gr_unittest.run(test_type_conversions, "test_type_conversions.xml")
