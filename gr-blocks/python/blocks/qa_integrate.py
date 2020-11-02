#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_integrate (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_ss(self):
        src_data = [1, 2, 3, 4, 5, 6]
        dst_data = [6, 15]
        src = blocks.vector_source_s(src_data)
        itg = blocks.integrate_ss(3)
        dst = blocks.vector_sink_s()
        self.tb.connect(src, itg, dst)
        self.tb.run()
        self.assertEqual(dst_data, dst.data())

    def test_001_ii(self):
        src_data = [1, 2, 3, 4, 5, 6]
        dst_data = [6, 15]
        src = blocks.vector_source_i(src_data)
        itg = blocks.integrate_ii(3)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, itg, dst)
        self.tb.run()
        self.assertEqual(dst_data, dst.data())

    def test_002_ff(self):
        src_data = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0]
        dst_data = [6.0, 15.0]
        src = blocks.vector_source_f(src_data)
        itg = blocks.integrate_ff(3)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, itg, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(dst_data, dst.data(), 6)

    def test_003_cc(self):
        src_data = [
            1.0 + 1.0j,
            2.0 + 2.0j,
            3.0 + 3.0j,
            4.0 + 4.0j,
            5.0 + 5.0j,
            6.0 + 6.0j]
        dst_data = [6.0 + 6.0j, 15.0 + 15.0j]
        src = blocks.vector_source_c(src_data)
        itg = blocks.integrate_cc(3)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, itg, dst)
        self.tb.run()
        self.assertComplexTuplesAlmostEqual(dst_data, dst.data(), 6)

    def test_004_ss_vec(self):
        src_data = [1, 2, 3, 4, 5, 6]
        dst_data = [9, 12]
        vlen = 2
        src = blocks.vector_source_s(src_data, False, vlen)
        itg = blocks.integrate_ss(3, vlen)
        dst = blocks.vector_sink_s(vlen)
        self.tb.connect(src, itg, dst)
        self.tb.run()
        self.assertEqual(dst_data, dst.data())

    def test_003_cc_vec(self):
        src_data = [
            1.0 + 1.0j,
            2.0 + 2.0j,
            3.0 + 3.0j,
            4.0 + 4.0j,
            5.0 + 5.0j,
            6.0 + 6.0j]
        dst_data = [9.0 + 9.0j, 12.0 + 12.0j]
        vlen = 2
        src = blocks.vector_source_c(src_data, False, vlen)
        itg = blocks.integrate_cc(3, vlen)
        dst = blocks.vector_sink_c(vlen)
        self.tb.connect(src, itg, dst)
        self.tb.run()
        self.assertComplexTuplesAlmostEqual(dst_data, dst.data(), 6)


if __name__ == '__main__':
    gr_unittest.run(test_integrate)
