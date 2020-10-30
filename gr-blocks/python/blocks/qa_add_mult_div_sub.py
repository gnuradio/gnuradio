#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_add_mult_div_sub(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def help_ii(self, src_data, exp_data, op):
        for s in zip(list(range(len(src_data))), src_data):
            src = blocks.vector_source_i(s[1])
            self.tb.connect(src, (op, s[0]))
        dst = blocks.vector_sink_i()
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_ss(self, src_data, exp_data, op):
        for s in zip(list(range(len(src_data))), src_data):
            src = blocks.vector_source_s(s[1])
            self.tb.connect(src, (op, s[0]))
        dst = blocks.vector_sink_s()
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_ff(self, src_data, exp_data, op):
        for s in zip(list(range(len(src_data))), src_data):
            src = blocks.vector_source_f(s[1])
            self.tb.connect(src, (op, s[0]))
        dst = blocks.vector_sink_f()
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def help_cc(self, src_data, exp_data, op):
        for s in zip(list(range(len(src_data))), src_data):
            src = blocks.vector_source_c(s[1])
            self.tb.connect(src, (op, s[0]))
        dst = blocks.vector_sink_c()
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    # add_XX

    def test_add_ss(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [9, -1, 7, 12, 7]
        op = blocks.add_ss()
        self.help_ss((src1_data, src2_data), expected_result, op)

    def test_add_ii(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [9, -1, 7, 12, 7]
        op = blocks.add_ii()
        self.help_ii((src1_data, src2_data), expected_result, op)

    def test_add_ff(self):
        src1_data = [1.0, 2.0, 3.0, 4.0, 5.0]
        src2_data = [8.0, -3.0, 4.0, 8.0, 2.0]
        expected_result = [9.0, -1.0, 7.0, 12.0, 7.0]
        op = blocks.add_ff()
        self.help_ff((src1_data, src2_data), expected_result, op)

    def test_add_cc(self):
        src1_data = [1 + 1j, 2 + 2j, 3 + 3j, 4 + 4j, 5 + 5j]
        src2_data = [8 + 8j, -3 - 3j, 4 + 4j, 8 + 8j, 2 + 2j]
        expected_result = [9 + 9j, -1 - 1j, 7 + 7j, 12 + 12j, 7 + 7j]
        op = blocks.add_cc()
        self.help_cc((src1_data, src2_data), expected_result, op)

    # add_const_XX

    def test_add_const_ss(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [6, 7, 8, 9, 10]
        op = blocks.add_const_ss(5)
        self.help_ss((src_data,), expected_result, op)

    def test_add_const_ii(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [6, 7, 8, 9, 10]
        op = blocks.add_const_ii(5)
        self.help_ii((src_data,), expected_result, op)

    def test_add_const_ff(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [6, 7, 8, 9, 10]
        op = blocks.add_const_ff(5)
        self.help_ff((src_data,), expected_result, op)

    def test_add_const_cc(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [1 + 5j, 2 + 5j, 3 + 5j, 4 + 5j, 5 + 5j]
        op = blocks.add_const_cc(5j)
        self.help_cc((src_data,), expected_result, op)

    # multiply_XX

    def test_multiply_ss(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [8, -6, 12, 32, 10]
        op = blocks.multiply_ss()
        self.help_ss((src1_data, src2_data),
                     expected_result, op)

    def test_multiply_ii(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [8, -6, 12, 32, 10]
        op = blocks.multiply_ii()
        self.help_ii((src1_data, src2_data),
                     expected_result, op)

    def test_multiply_ff(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [8, -6, 12, 32, 10]
        op = blocks.multiply_ff()
        self.help_ff((src1_data, src2_data),
                     expected_result, op)

    def test_multiply_cc(self):
        src1_data = [1 + 1j, 2 + 2j, 3 + 3j, 4 + 4j, 5 + 5j]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [8 + 8j, -6 - 6j, 12 + 12j, 32 + 32j, 10 + 10j]
        op = blocks.multiply_cc()
        self.help_cc((src1_data, src2_data),
                     expected_result, op)

    # multiply_const_XX

    def test_multiply_const_ss(self):
        src_data = [-1, 0, 1, 2, 3]
        expected_result = [-5, 0, 5, 10, 15]
        op = blocks.multiply_const_ss(5)
        self.help_ss((src_data,), expected_result, op)

    def test_multiply_const_ii(self):
        src_data = [-1, 0, 1, 2, 3]
        expected_result = [-5, 0, 5, 10, 15]
        op = blocks.multiply_const_ii(5)
        self.help_ii((src_data,), expected_result, op)

    def test_multiply_const_ff(self):
        src_data = [-1, 0, 1, 2, 3]
        expected_result = [-5, 0, 5, 10, 15]
        op = blocks.multiply_const_ff(5)
        self.help_ff((src_data,), expected_result, op)

    def test_multiply_const_cc(self):
        src_data = [-1 - 1j, 0 + 0j, 1 + 1j, 2 + 2j, 3 + 3j]
        expected_result = [-5 - 5j, 0 + 0j, 5 + 5j, 10 + 10j, 15 + 15j]
        op = blocks.multiply_const_cc(5)
        self.help_cc((src_data,), expected_result, op)

    def test_multiply_const_cc2(self):
        src_data = [-1 - 1j, 0 + 0j, 1 + 1j, 2 + 2j, 3 + 3j]
        expected_result = [-3 - 7j, 0 + 0j, 3 + 7j, 6 + 14j, 9 + 21j]
        op = blocks.multiply_const_cc(5 + 2j)
        self.help_cc((src_data,), expected_result, op)

    def test_sub_ii(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [-7, 5, -1, -4, 3]
        op = blocks.sub_ii()
        self.help_ii((src1_data, src2_data),
                     expected_result, op)

    def test_sub_ii1(self):
        src1_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]

        src = blocks.vector_source_i(src1_data)
        op = blocks.sub_ii()
        dst = blocks.vector_sink_i()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_sub_ss(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [-7, 5, -1, -4, 3]
        op = blocks.sub_ss()
        self.help_ss((src1_data, src2_data),
                     expected_result, op)

    def test_sub_ss1(self):
        src1_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]

        src = blocks.vector_source_s(src1_data)
        op = blocks.sub_ss()
        dst = blocks.vector_sink_s()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_sub_ff(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [-7, 5, -1, -4, 3]
        op = blocks.sub_ff()
        self.help_ff((src1_data, src2_data),
                     expected_result, op)

    def test_sub_ff1(self):
        src1_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]

        src = blocks.vector_source_f(src1_data)
        op = blocks.sub_ff()
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_sub_cc(self):
        src1_data = [1, 2, 3, 4, 5]
        src2_data = [8, -3, 4, 8, 2]
        expected_result = [-7, 5, -1, -4, 3]
        op = blocks.sub_cc()
        self.help_cc((src1_data, src2_data),
                     expected_result, op)

    def test_sub_cc1(self):
        src1_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]

        src = blocks.vector_source_c(src1_data)
        op = blocks.sub_cc()
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_div_ff(self):
        src1_data = [5, 9, -15, 1024]
        src2_data = [10, 3, -5, 64]
        expected_result = [0.5, 3, 3, 16]
        op = blocks.divide_ff()
        self.help_ff((src1_data, src2_data), expected_result, op)


if __name__ == '__main__':
    gr_unittest.run(test_add_mult_div_sub)
