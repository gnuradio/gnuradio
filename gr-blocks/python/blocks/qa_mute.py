#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2010,2013 Free Software Foundation, Inc.
# Copyright 2024 Skandalis Georgios
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import pmt


class test_mute(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_ff_mute(self):
        src_arr = [1.0 for i in range(50)]
        src = blocks.vector_source_f(src_arr, False)
        dst = blocks.vector_sink_f()
        mute = blocks.mute_ff(False)

        self.tb.connect(src, mute, dst)

        mute._post(pmt.intern("set_mute"), pmt.from_bool(True))

        self.tb.run()
        self.tb.stop()
        self.tb.wait()

        self.assertFalse(1.0 in dst.data())

    def test_ff_unmute(self):
        src_arr = [1.0 for i in range(50)]
        src = blocks.vector_source_f(src_arr, False)
        dst = blocks.vector_sink_f()
        mute = blocks.mute_ff(False)

        self.tb.connect(src, mute, dst)

        mute._post(pmt.intern("set_mute"), pmt.from_bool(False))

        self.tb.run()
        self.tb.stop()
        self.tb.wait()

        self.assertFalse(0.0 in dst.data())

    def test_ff_mute_pdu(self):
        src_arr = [1.0 for i in range(50)]
        src = blocks.vector_source_f(src_arr, False)
        dst = blocks.vector_sink_f()
        mute = blocks.mute_ff(False)

        self.tb.connect(src, mute, dst)

        mute._post(pmt.intern("set_mute"), pmt.cons(pmt.PMT_NIL, pmt.from_bool(True)))

        self.tb.run()
        self.tb.stop()
        self.tb.wait()

        self.assertFalse(1.0 in dst.data())

    def test_ff_unmute_pdu(self):
        src_arr = [1.0 for i in range(50)]
        src = blocks.vector_source_f(src_arr, False)
        dst = blocks.vector_sink_f()
        mute = blocks.mute_ff(True)

        self.tb.connect(src, mute, dst)

        mute._post(pmt.intern("set_mute"), pmt.cons(pmt.PMT_NIL, pmt.from_bool(False)))

        self.tb.run()
        self.tb.stop()
        self.tb.wait()

        self.assertFalse(0.0 in dst.data())

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

    def help_cc(self, src_data, exp_data, op):
        for s in zip(list(range(len(src_data))), src_data):
            src = blocks.vector_source_c(s[1])
            self.tb.connect(src, (op, s[0]))
        dst = blocks.vector_sink_c()
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(exp_data, result_data)

    def test_unmute_ii(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]
        op = blocks.mute_ii(False)
        self.help_ii((src_data,), expected_result, op)

    def test_mute_ii(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [0, 0, 0, 0, 0]
        op = blocks.mute_ii(True)
        self.help_ii((src_data,), expected_result, op)

    def test_unmute_cc(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]
        op = blocks.mute_ss(False)
        self.help_ss((src_data,), expected_result, op)

    def test_mute_cc(self):
        src_data = [1, 2, 3, 4, 5]
        expected_result = [1, 2, 3, 4, 5]
        op = blocks.mute_ss(True)
        self.help_ss((src_data,), expected_result, op)

    def test_unmute_cc(self):
        src_data = [1 + 5j, 2 + 5j, 3 + 5j, 4 + 5j, 5 + 5j]
        expected_result = [1 + 5j, 2 + 5j, 3 + 5j, 4 + 5j, 5 + 5j]
        op = blocks.mute_cc(False)
        self.help_cc((src_data,), expected_result, op)

    def test_mute_cc(self):
        src_data = [1 + 5j, 2 + 5j, 3 + 5j, 4 + 5j, 5 + 5j]
        expected_result = [0 + 0j, 0 + 0j, 0 + 0j, 0 + 0j, 0 + 0j]
        op = blocks.mute_cc(True)
        self.help_cc((src_data,), expected_result, op)


if __name__ == '__main__':
    gr_unittest.run(test_mute)
