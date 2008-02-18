#!/usr/bin/env python
#
# Copyright 2004,2007,2008 Free Software Foundation, Inc.
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

class test_head (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def help_ss (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = gr.vector_source_s (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = gr.vector_sink_s ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def help_bb (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = gr.vector_source_b (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = gr.vector_sink_b ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def help_ii (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = gr.vector_source_i (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = gr.vector_sink_i ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)
        
    def test_xor_ss (self):
        src1_data =       (1,  2,  3,  0x5004,   0x1150)
        src2_data =       (8,  2,  1 , 0x0508,   0x1105)
        expected_result = (9,  0,  2,  0x550C,   0x0055)
        op = gr.xor_ss ()
        self.help_ss ((src1_data, src2_data),
                      expected_result, op)

    def test_xor_bb (self):
        src1_data =       (1,  2,  3,  4,   0x50)
        src2_data =       (8,  2,  1 , 8,   0x05)
        expected_result = (9,  0,  2,  0xC, 0x55)
        op = gr.xor_bb ()
        self.help_bb ((src1_data, src2_data),
                      expected_result, op)


    def test_xor_ii (self):
        src1_data =       (1,  2,  3,  0x5000004,   0x11000050)
        src2_data =       (8,  2,  1 , 0x0500008,   0x11000005)
        expected_result = (9,  0,  2,  0x550000C,   0x00000055)
        op = gr.xor_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)



if __name__ == '__main__':
    gr_unittest.main ()
