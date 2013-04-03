#!/usr/bin/env python
#
# Copyright 2004,2007,2008,2010,2013 Free Software Foundation, Inc.
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

class test_boolean_operators (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def help_ss (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = blocks.vector_source_s (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = blocks.vector_sink_s ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def help_bb (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = blocks.vector_source_b (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = blocks.vector_sink_b ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def help_ii (self, src_data, exp_data, op):
        for s in zip (range (len (src_data)), src_data):
            src = blocks.vector_source_i (s[1])
            self.tb.connect (src, (op, s[0]))
        dst = blocks.vector_sink_i ()
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

    def test_xor_ss (self):
        src1_data =       (1,  2,  3,  0x5004,   0x1150)
        src2_data =       (8,  2,  1 , 0x0508,   0x1105)
        expected_result = (9,  0,  2,  0x550C,   0x0055)
        op = blocks.xor_ss ()
        self.help_ss ((src1_data, src2_data),
                      expected_result, op)

    def test_xor_bb (self):
        src1_data =       (1,  2,  3,  4,   0x50)
        src2_data =       (8,  2,  1 , 8,   0x05)
        expected_result = (9,  0,  2,  0xC, 0x55)
        op = blocks.xor_bb ()
        self.help_bb ((src1_data, src2_data),
                      expected_result, op)


    def test_xor_ii (self):
        src1_data =       (1,  2,  3,  0x5000004,   0x11000050)
        src2_data =       (8,  2,  1 , 0x0500008,   0x11000005)
        expected_result = (9,  0,  2,  0x550000C,   0x00000055)
        op = blocks.xor_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)


    def test_and_ss (self):
        src1_data =       (1,  2,  3,  0x5004,   0x1150)
        src2_data =       (8,  2,  1 , 0x0508,   0x1105)
        expected_result = (0,  2,  1,  0x0000,   0x1100)
        op = blocks.and_ss ()
        self.help_ss ((src1_data, src2_data),
                      expected_result, op)

    def test_and_bb (self):
        src1_data =       (1,  2, 2,  3,  0x04,   0x50)
        src2_data =       (8,  2, 2,  1,  0x08,   0x05)
        src3_data =       (8,  2, 1,  1,  0x08,   0x05)
        expected_result = (0,  2, 0,  1,  0x00,   0x00)
        op = blocks.and_bb ()
        self.help_bb ((src1_data, src2_data, src3_data),
                      expected_result, op)

    def test_and_ii (self):
        src1_data =       (1,  2,  3,  0x50005004,   0x11001150)
        src2_data =       (8,  2,  1 , 0x05000508,   0x11001105)
        expected_result = (0,  2,  1,  0x00000000,   0x11001100)
        op = blocks.and_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)

    def test_and_const_ss (self):
        src_data =        (1,  2,  3,  0x5004,   0x1150)
        expected_result = (0,  2,  2,  0x5000,   0x1100)
        src = blocks.vector_source_s(src_data)
        op = blocks.and_const_ss (0x55AA)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(dst.data(), expected_result)

    def test_and_const_bb (self):
        src_data =        (1,  2, 3,  0x50,   0x11)
        expected_result = (0,  2, 2,  0x00,   0x00)
        src = blocks.vector_source_b(src_data)
        op = blocks.and_const_bb (0xAA)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(dst.data(), expected_result)


    def test_and_const_ii (self):
        src_data =        (1,  2,  3,  0x5004,   0x1150)
        expected_result = (0,  2,  2,  0x5000,   0x1100)
        src = blocks.vector_source_i(src_data)
        op = blocks.and_const_ii (0x55AA)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(dst.data(), expected_result)


    def test_or_ss (self):
        src1_data =       (1,  2,  3,  0x5004,   0x1150)
        src2_data =       (8,  2,  1 , 0x0508,   0x1105)
        expected_result = (9,  2,  3,  0x550C,   0x1155)
        op = blocks.or_ss ()
        self.help_ss ((src1_data, src2_data),
                      expected_result, op)

    def test_or_bb (self):
        src1_data =       (1,  2, 2,  3,  0x04,   0x50)
        src2_data =       (8,  2, 2,  1 , 0x08,   0x05)
        src3_data =       (8,  2, 1,  1 , 0x08,   0x05)
        expected_result = (9,  2, 3,  3,  0x0C,   0x55)
        op = blocks.or_bb ()
        self.help_bb ((src1_data, src2_data, src3_data),
                      expected_result, op)

    def test_or_ii (self):
        src1_data =       (1,  2,  3,  0x50005004,   0x11001150)
        src2_data =       (8,  2,  1 , 0x05000508,   0x11001105)
        expected_result = (9,  2,  3,  0x5500550C,   0x11001155)
        op = blocks.or_ii ()
        self.help_ii ((src1_data, src2_data),
                      expected_result, op)

    def test_not_ss (self):
        src1_data =       (1,      2,      3,       0x5004,   0x1150)
        expected_result = (~1,     ~2,      ~3,       ~0x5004,   ~0x1150)
        op = blocks.not_ss ()
        self.help_ss ((((src1_data),)),
                      expected_result, op)

    def test_not_bb (self):
        src1_data =       (1,     2,    2,     3,     0x04,   0x50)
        expected_result = (0xFE,  0xFD, 0xFD,  0xFC,  0xFB,   0xAF)
        op = blocks.not_bb ()
        self.help_bb (((src1_data), ),
                      expected_result, op)

    def test_not_ii (self):
        src1_data =       (1,    2,  3,  0x50005004,   0x11001150)
        expected_result = (~1 , ~2, ~3, ~0x50005004,  ~0x11001150)
        op = blocks.not_ii ()
        self.help_ii (((src1_data),),
                      expected_result, op)


if __name__ == '__main__':
    gr_unittest.run(test_boolean_operators, "test_boolean_operators.xml")
