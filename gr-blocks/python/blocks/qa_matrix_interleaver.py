#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 Caliola Engineering, LLC.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks


class qa_matrix_interleaver(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_interleave(self):
        tb = self.tb

        # set up fg
        cols, rows = 4, 10
        vec = sum((cols * [x, ] for x in range(rows)), [])
        expected = cols * list(range(rows))

        src = blocks.vector_source_f(vec, False)
        itlv = blocks.matrix_interleaver(gr.sizeof_float, rows=rows, cols=cols)
        snk = blocks.vector_sink_f()

        tb.connect(src, itlv, snk)
        tb.run()
        result = snk.data()

        # check data
        self.assertFloatTuplesAlmostEqual(expected, result)

    def test_deinterleave(self):
        tb = self.tb

        # set up fg
        cols, rows = 4, 10
        vec = sum((rows * [x, ] for x in range(cols)), [])
        expected = rows * list(range(cols))

        src = blocks.vector_source_f(vec, False)
        itlv = blocks.matrix_interleaver(
            gr.sizeof_float, rows=rows, cols=cols, deint=True)
        snk = blocks.vector_sink_f()

        tb.connect(src, itlv, snk)
        tb.run()
        result = snk.data()

        # check data
        self.assertFloatTuplesAlmostEqual(expected, result)


if __name__ == '__main__':
    gr_unittest.run(qa_matrix_interleaver)
