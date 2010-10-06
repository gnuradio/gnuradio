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

class test_add_v_and_friends(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def help_ss(self, size, src_data, exp_data, op):
        for s in zip(range (len (src_data)), src_data):
            src = gr.vector_source_s(s[1])
	    srcv = gr.stream_to_vector(gr.sizeof_short, size)
	    self.tb.connect(src, srcv)
            self.tb.connect(srcv, (op, s[0]))
	rhs = gr.vector_to_stream(gr.sizeof_short, size)
        dst = gr.vector_sink_s()
        self.tb.connect(op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_ii(self, size, src_data, exp_data, op):
        for s in zip(range (len (src_data)), src_data):
            src = gr.vector_source_i(s[1])
	    srcv = gr.stream_to_vector(gr.sizeof_int, size)
	    self.tb.connect(src, srcv)
            self.tb.connect(srcv, (op, s[0]))
	rhs = gr.vector_to_stream(gr.sizeof_int, size)
        dst = gr.vector_sink_i()
        self.tb.connect(op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_ff(self, size, src_data, exp_data, op):
        for s in zip(range (len (src_data)), src_data):
            src = gr.vector_source_f(s[1])
	    srcv = gr.stream_to_vector(gr.sizeof_float, size)
	    self.tb.connect(src, srcv)
            self.tb.connect(srcv, (op, s[0]))
	rhs = gr.vector_to_stream(gr.sizeof_float, size)
        dst = gr.vector_sink_f()
        self.tb.connect(op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_cc(self, size, src_data, exp_data, op):
        for s in zip(range (len (src_data)), src_data):
            src = gr.vector_source_c(s[1])
	    srcv = gr.stream_to_vector(gr.sizeof_gr_complex, size)
	    self.tb.connect(src, srcv)
            self.tb.connect(srcv, (op, s[0]))
	rhs = gr.vector_to_stream(gr.sizeof_gr_complex, size)
        dst = gr.vector_sink_c()
        self.tb.connect(op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_const_ss(self, src_data, exp_data, op):
	src = gr.vector_source_s(src_data)
	srcv = gr.stream_to_vector(gr.sizeof_short, len(src_data))
	rhs = gr.vector_to_stream(gr.sizeof_short, len(src_data))
        dst = gr.vector_sink_s()
        self.tb.connect(src, srcv, op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)
	
    def help_const_ii(self, src_data, exp_data, op):
	src = gr.vector_source_i(src_data)
	srcv = gr.stream_to_vector(gr.sizeof_int, len(src_data))
	rhs = gr.vector_to_stream(gr.sizeof_int, len(src_data))
        dst = gr.vector_sink_i()
        self.tb.connect(src, srcv, op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)
	
    def help_const_ff(self, src_data, exp_data, op):
	src = gr.vector_source_f(src_data)
	srcv = gr.stream_to_vector(gr.sizeof_float, len(src_data))
	rhs = gr.vector_to_stream(gr.sizeof_float, len(src_data))
        dst = gr.vector_sink_f()
        self.tb.connect(src, srcv, op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)
	
    def help_const_cc(self, src_data, exp_data, op):
	src = gr.vector_source_c(src_data)
	srcv = gr.stream_to_vector(gr.sizeof_gr_complex, len(src_data))
	rhs = gr.vector_to_stream(gr.sizeof_gr_complex, len(src_data))
        dst = gr.vector_sink_c()
        self.tb.connect(src, srcv, op, rhs, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)
	

    def test_add_vss_one(self):
	src1_data = (1,)
	src2_data = (2,)
	src3_data = (3,)
	expected_result = (6,)
	op = gr.add_vss(1)
	self.help_ss(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_add_vss_five(self):
	src1_data = (1, 2, 3, 4, 5)
	src2_data = (6, 7, 8, 9, 10)
	src3_data = (11, 12, 13, 14, 15)
	expected_result = (18, 21, 24, 27, 30)
	op = gr.add_vss(5)
	self.help_ss(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_add_vii_one(self):
	src1_data = (1,)
	src2_data = (2,)
	src3_data = (3,)
	expected_result = (6,)
	op = gr.add_vii(1)
	self.help_ii(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_add_vii_five(self):
	src1_data = (1, 2, 3, 4, 5)
	src2_data = (6, 7, 8, 9, 10)
	src3_data = (11, 12, 13, 14, 15)
	expected_result = (18, 21, 24, 27, 30)
	op = gr.add_vii(5)
	self.help_ii(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_add_vff_one(self):
	src1_data = (1.0,)
	src2_data = (2.0,)
	src3_data = (3.0,)
	expected_result = (6.0,)
	op = gr.add_vff(1)
	self.help_ff(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_add_vff_five(self):
	src1_data = (1.0, 2.0, 3.0, 4.0, 5.0)
	src2_data = (6.0, 7.0, 8.0, 9.0, 10.0)
	src3_data = (11.0, 12.0, 13.0, 14.0, 15.0)
	expected_result = (18.0, 21.0, 24.0, 27.0, 30.0)
	op = gr.add_vff(5)
	self.help_ff(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_add_vcc_one(self):
	src1_data = (1.0+2.0j,)
	src2_data = (3.0+4.0j,)
	src3_data = (5.0+6.0j,)
	expected_result = (9.0+12j,)
	op = gr.add_vcc(1)
	self.help_cc(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_add_vcc_five(self):
	src1_data = (1.0+2.0j, 3.0+4.0j, 5.0+6.0j, 7.0+8.0j, 9.0+10.0j)
	src2_data = (11.0+12.0j, 13.0+14.0j, 15.0+16.0j, 17.0+18.0j, 19.0+20.0j)
	src3_data = (21.0+22.0j, 23.0+24.0j, 25.0+26.0j, 27.0+28.0j, 29.0+30.0j)
	expected_result = (33.0+36.0j, 39.0+42.0j, 45.0+48.0j, 51.0+54.0j, 57.0+60.0j)
	op = gr.add_vcc(5)
	self.help_cc(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_add_const_vss_one(self):
	src_data = (1,)
	op = gr.add_const_vss((2,))
	exp_data = (3,)
	self.help_const_ss(src_data, exp_data, op)

    def test_add_const_vss_five(self):
	src_data = (1, 2, 3, 4, 5)
	op = gr.add_const_vss((6, 7, 8, 9, 10))
	exp_data = (7, 9, 11, 13, 15)
	self.help_const_ss(src_data, exp_data, op)

    def test_add_const_vii_one(self):
	src_data = (1,)
	op = gr.add_const_vii((2,))
	exp_data = (3,)
	self.help_const_ii(src_data, exp_data, op)

    def test_add_const_vii_five(self):
	src_data = (1, 2, 3, 4, 5)
	op = gr.add_const_vii((6, 7, 8, 9, 10))
	exp_data = (7, 9, 11, 13, 15)
	self.help_const_ii(src_data, exp_data, op)

    def test_add_const_vff_one(self):
	src_data = (1.0,)
	op = gr.add_const_vff((2.0,))
	exp_data = (3.0,)
	self.help_const_ff(src_data, exp_data, op)

    def test_add_const_vff_five(self):
	src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
	op = gr.add_const_vff((6.0, 7.0, 8.0, 9.0, 10.0))
	exp_data = (7.0, 9.0, 11.0, 13.0, 15.0)
	self.help_const_ff(src_data, exp_data, op)

    def test_add_const_vcc_one(self):
	src_data = (1.0+2.0j,)
	op = gr.add_const_vcc((2.0+3.0j,))
	exp_data = (3.0+5.0j,)
	self.help_const_cc(src_data, exp_data, op)

    def test_add_const_vcc_five(self):
	src_data = (1.0+2.0j, 3.0+4.0j, 5.0+6.0j, 7.0+8.0j, 9.0+10.0j)
	op = gr.add_const_vcc((11.0+12.0j, 13.0+14.0j, 15.0+16.0j, 17.0+18.0j, 19.0+20.0j))
	exp_data = (12.0+14.0j, 16.0+18.0j, 20.0+22.0j, 24.0+26.0j, 28.0+30.0j)
	self.help_const_cc(src_data, exp_data, op)


    def test_multiply_vss_one(self):
	src1_data = (1,)
	src2_data = (2,)
	src3_data = (3,)
	expected_result = (6,)
	op = gr.multiply_vss(1)
	self.help_ss(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_multiply_vss_five(self):
	src1_data = (1, 2, 3, 4, 5)
	src2_data = (6, 7, 8, 9, 10)
	src3_data = (11, 12, 13, 14, 15)
	expected_result = (66, 168, 312, 504, 750)
	op = gr.multiply_vss(5)
	self.help_ss(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_multiply_vii_one(self):
	src1_data = (1,)
	src2_data = (2,)
	src3_data = (3,)
	expected_result = (6,)
	op = gr.multiply_vii(1)
	self.help_ii(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_multiply_vii_five(self):
	src1_data = (1, 2, 3, 4, 5)
	src2_data = (6, 7, 8, 9, 10)
	src3_data = (11, 12, 13, 14, 15)
	expected_result = (66, 168, 312, 504, 750)
	op = gr.multiply_vii(5)
	self.help_ii(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_multiply_vff_one(self):
	src1_data = (1.0,)
	src2_data = (2.0,)
	src3_data = (3.0,)
	expected_result = (6.0,)
	op = gr.multiply_vff(1)
	self.help_ff(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_multiply_vff_five(self):
	src1_data = (1.0, 2.0, 3.0, 4.0, 5.0)
	src2_data = (6.0, 7.0, 8.0, 9.0, 10.0)
	src3_data = (11.0, 12.0, 13.0, 14.0, 15.0)
	expected_result = (66.0, 168.0, 312.0, 504.0, 750.0)
	op = gr.multiply_vff(5)
	self.help_ff(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_multiply_vcc_one(self):
	src1_data = (1.0+2.0j,)
	src2_data = (3.0+4.0j,)
	src3_data = (5.0+6.0j,)
	expected_result = (-85+20j,)
	op = gr.multiply_vcc(1)
	self.help_cc(1, (src1_data, src2_data, src3_data), expected_result, op)
	
    def test_multiply_vcc_five(self):
	src1_data = (1.0+2.0j, 3.0+4.0j, 5.0+6.0j, 7.0+8.0j, 9.0+10.0j)
	src2_data = (11.0+12.0j, 13.0+14.0j, 15.0+16.0j, 17.0+18.0j, 19.0+20.0j)
	src3_data = (21.0+22.0j, 23.0+24.0j, 25.0+26.0j, 27.0+28.0j, 29.0+30.0j)
	expected_result = (-1021.0+428.0j, -2647.0+1754.0j, -4945.0+3704.0j, -8011.0+6374.0j, -11941.0+9860.0j)
	op = gr.multiply_vcc(5)
	self.help_cc(5, (src1_data, src2_data, src3_data), expected_result, op)

    def test_multiply_const_vss_one(self):
	src_data = (2,)
	op = gr.multiply_const_vss((3,))
	exp_data = (6,)
	self.help_const_ss(src_data, exp_data, op)

    def test_multiply_const_vss_five(self):
	src_data = (1, 2, 3, 4, 5)
	op = gr.multiply_const_vss((6, 7, 8, 9, 10))
	exp_data = (6, 14, 24, 36, 50)
	self.help_const_ss(src_data, exp_data, op)

    def test_multiply_const_vii_one(self):
	src_data = (2,)
	op = gr.multiply_const_vii((3,))
	exp_data = (6,)
	self.help_const_ii(src_data, exp_data, op)

    def test_multiply_const_vii_five(self):
	src_data = (1, 2, 3, 4, 5)
	op = gr.multiply_const_vii((6, 7, 8, 9, 10))
	exp_data = (6, 14, 24, 36, 50)
	self.help_const_ii(src_data, exp_data, op)

    def test_multiply_const_vff_one(self):
	src_data = (2.0,)
	op = gr.multiply_const_vff((3.0,))
	exp_data = (6.0,)
	self.help_const_ff(src_data, exp_data, op)

    def test_multiply_const_vff_five(self):
	src_data = (1.0, 2.0, 3.0, 4.0, 5.0)
	op = gr.multiply_const_vff((6.0, 7.0, 8.0, 9.0, 10.0))
	exp_data = (6.0, 14.0, 24.0, 36.0, 50.0)
	self.help_const_ff(src_data, exp_data, op)

    def test_multiply_const_vcc_one(self):
	src_data = (1.0+2.0j,)
	op = gr.multiply_const_vcc((2.0+3.0j,))
	exp_data = (-4.0+7.0j,)
	self.help_const_cc(src_data, exp_data, op)

    def test_multiply_const_vcc_five(self):
	src_data = (1.0+2.0j, 3.0+4.0j, 5.0+6.0j, 7.0+8.0j, 9.0+10.0j)
	op = gr.multiply_const_vcc((11.0+12.0j, 13.0+14.0j, 15.0+16.0j, 17.0+18.0j, 19.0+20.0j))
	exp_data = (-13.0+34.0j, -17.0+94.0j, -21.0+170.0j, -25.0+262.0j, -29.0+370.0j)
	self.help_const_cc(src_data, exp_data, op)


if __name__ == '__main__':
    gr_unittest.run(test_add_v_and_friends, "test_add_v_and_friends.xml")
