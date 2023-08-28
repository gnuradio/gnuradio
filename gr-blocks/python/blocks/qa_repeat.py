#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import numpy as np


class test_repeat(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def run_fg(self, N, r, msg=""):
        src_data = np.arange(N)
        src = blocks.vector_source_i(src_data)
        rpt = blocks.repeat(gr.sizeof_int, r)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, rpt, dst)
        self.tb.run()
        self.assertTrue(np.array_equal(dst.data(), src_data.repeat(r)), msg)

    def test_001_various_sizes(self):
        sizes = ((3**10 + 2, 3), (3**6, 1), (4, 5), (10**6, 1), (10 * 2, 10 * 3))
        for size, repetitions in sizes:
            self.run_fg(size, repetitions, f"N = {size}, r = {repetitions}: not equal")


if __name__ == "__main__":
    gr_unittest.run(test_repeat)
