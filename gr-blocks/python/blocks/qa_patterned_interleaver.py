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

import math


class test_patterned_interleaver (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        dst_data = [0, 0, 1, 2, 0, 2, 1, 0]
        src0 = blocks.vector_source_f(200 * [0])
        src1 = blocks.vector_source_f(200 * [1])
        src2 = blocks.vector_source_f(200 * [2])
        itg = blocks.patterned_interleaver(gr.sizeof_float, dst_data)
        dst = blocks.vector_sink_f()
        head = blocks.head(gr.sizeof_float, 8)

        self.tb.connect(src0, (itg, 0))
        self.tb.connect(src1, (itg, 1))
        self.tb.connect(src2, (itg, 2))
        self.tb.connect(itg, head, dst)

        self.tb.run()
        self.assertEqual(list(dst_data), list(dst.data()))


if __name__ == '__main__':
    gr_unittest.run(test_patterned_interleaver)
