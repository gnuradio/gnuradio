#!/usr/bin/env python
#
# Copyright 2004,2007,2010 Free Software Foundation, Inc.
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
import math

class test_complex_ops (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_complex_to_float_1 (self):
        src_data = (0, 1, -1, 3+4j, -3-4j, -3+4j)
        expected_result = (0, 1, -1, 3, -3, -3)
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_float ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()               # run the graph and wait for it to finish
        actual_result = dst.data ()  # fetch the contents of the sink
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result)

    def test_complex_to_float_2 (self):
        src_data = (0, 1, -1, 3+4j, -3-4j, -3+4j)
        expected_result0 = (0, 1, -1, 3, -3, -3)
        expected_result1 = (0, 0, 0, 4, -4, 4)
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_float ()
        dst0 = gr.vector_sink_f ()
        dst1 = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect ((op, 0), dst0)
        self.tb.connect ((op, 1), dst1)
        self.tb.run ()
        actual_result = dst0.data ()
        self.assertFloatTuplesAlmostEqual (expected_result0, actual_result)
        actual_result = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result1, actual_result)

    def test_complex_to_real (self):
        src_data = (0, 1, -1, 3+4j, -3-4j, -3+4j)
        expected_result = (0, 1, -1, 3, -3, -3)
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_real ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        actual_result = dst.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result)

    def test_complex_to_imag (self):
        src_data = (0, 1, -1, 3+4j, -3-4j, -3+4j)
        expected_result = (0, 0, 0, 4, -4, 4)
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_imag ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        actual_result = dst.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result,5)

    def test_complex_to_mag (self):
        src_data = (0, 1, -1, 3+4j, -3-4j, -3+4j)
        expected_result = (0, 1, 1, 5, 5, 5)
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_mag ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        actual_result = dst.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result,5)

    def test_complex_to_mag_squared (self):
        src_data = (0, 1, -1, 3+4j, -3-4j, -3+4j)
        expected_result = (0, 1, 1, 25, 25, 25)
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_mag_squared ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        actual_result = dst.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result,5)

    def test_complex_to_arg (self):
        pi = math.pi
        input_data = (0, pi/6, pi/4, pi/2, 3*pi/4, 7*pi/8,
                      -pi/6, -pi/4, -pi/2, -3*pi/4, -7*pi/8)

        expected_result = (0.0,                  # 0
                           0.52382522821426392,  # pi/6
                           0.78539806604385376,  # pi/4
                           1.5707963705062866,   # pi/2
                           2.3561947345733643,   # 3pi/4
                           2.7491819858551025,   # 7pi/8
                           -0.52382522821426392, # -pi/6
                           -0.78539806604385376, # -pi/4
                           -1.5707963705062866,  # -pi/2
                           -2.3561947345733643,  # -3pi/4
                           -2.7491819858551025)  # -7pi/8

        src_data = tuple ([math.cos (x) + math.sin (x) * 1j for x in input_data])
        src = gr.vector_source_c (src_data)
        op = gr.complex_to_arg ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        actual_result = dst.data ()

        self.assertFloatTuplesAlmostEqual (expected_result, actual_result, 3)


if __name__ == '__main__':
    gr_unittest.run(test_complex_ops, "test_complex_ops.xml")

