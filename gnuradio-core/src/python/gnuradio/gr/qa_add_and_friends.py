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

class test_add_and_friends (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def help_ii (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = gr.vector_source_i (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = gr.vector_sink_i ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def help_ff (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = gr.vector_source_f (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = gr.vector_sink_f ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def help_cc (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = gr.vector_source_c (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = gr.vector_sink_c ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def test_add_const_ii (self):
        src_data = (1, 2, 3, 4, 5)
        expected_result = (6, 7, 8, 9, 10)
        op = gr.add_const_ii (5)
        self.help_ii ((src_data,), expected_result, op)

    def test_add_const_cc (self):
        src_data = (1, 2, 3, 4, 5)
        expected_result = (1+5j, 2+5j, 3+5j, 4+5j, 5+5j)
        op = gr.add_const_cc (5j)
        self.help_cc ((src_data,), expected_result, op)

    def test_mult_const_ii (self):
        src_data = (-1, 0, 1, 2, 3)
        expected_result = (-5, 0, 5, 10, 15)
        op = gr.multiply_const_ii (5)
        self.help_ii ((src_data,), expected_result, op)

    def test_mult_const_ff (self):
        src_data = (-1, 0, 1, 2, 3)
        expected_result = (-5, 0, 5, 10, 15)
        op = gr.multiply_const_cc (5)
        self.help_cc ((src_data,), expected_result, op)

    def test_mult_const_cc (self):
        src_data = (-1-1j, 0+0j, 1+1j, 2+2j, 3+3j)
        expected_result = (-5-5j, 0+0j, 5+5j, 10+10j, 15+15j)
        op = gr.multiply_const_cc (5)
        self.help_cc ((src_data,), expected_result, op)

    def test_mult_const_cc2 (self):
        src_data = (-1-1j, 0+0j, 1+1j, 2+2j, 3+3j)
        expected_result = (-3-7j, 0+0j, 3+7j, 6+14j, 9+21j)
        op = gr.multiply_const_cc (5+2j)
        self.help_cc ((src_data,), expected_result, op)

    def test_add_ii (self):
        src1_data = (1,  2, 3, 4, 5)
        src2_data = (8, -3, 4, 8, 2)
        expected_result = (9, -1, 7, 12, 7)
        op = gr.add_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)

    def test_mult_ii (self):
        src1_data = (1,  2, 3, 4, 5)
        src2_data = (8, -3, 4, 8, 2)
        expected_result = (8, -6, 12, 32, 10)
        op = gr.multiply_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)

    def test_mult_ff (self):
        src1_data = (1,  2, 3, 4, 5)
        src2_data = (8, -3, 4, 8, 2)
        expected_result = (8, -6, 12, 32, 10)
        op = gr.multiply_ff ()
        self.help_ff ((src1_data, src2_data),
                      expected_result, op)

    def test_mult_cc (self):
        src1_data = (1+1j,  2+2j, 3+3j, 4+4j, 5+5j)
        src2_data = (8, -3, 4, 8, 2)
        expected_result = (8+8j, -6-6j, 12+12j, 32+32j, 10+10j)
        op = gr.multiply_cc ()
        self.help_cc ((src1_data, src2_data),
                      expected_result, op)

    def test_sub_ii_1 (self):
        src1_data = (1,  2, 3, 4, 5)
        expected_result = (-1, -2, -3, -4, -5)
        op = gr.sub_ii ()
        self.help_ii ((src1_data,),
                      expected_result, op)

    def test_sub_ii_2 (self):
        src1_data = (1,  2, 3, 4, 5)
        src2_data = (8, -3, 4, 8, 2)
        expected_result = (-7, 5, -1, -4, 3)
        op = gr.sub_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)

    def test_div_ff_1 (self):
        src1_data       = (1,  2,  4,    -8)
        expected_result = (1, 0.5, 0.25, -.125)
        op = gr.divide_ff ()
        self.help_ff ((src1_data,),
                      expected_result, op)

    def test_div_ff_2 (self):
        src1_data       = ( 5,  9, -15, 1024)
        src2_data       = (10,  3,  -5,   64)
        expected_result = (0.5, 3,   3,   16)
        op = gr.divide_ff ()
        self.help_ff ((src1_data, src2_data),
                      expected_result, op)


if __name__ == '__main__':
    gr_unittest.run(test_add_and_friends, "test_add_and_friends.xml")
