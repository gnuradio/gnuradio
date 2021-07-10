#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2020 Caliola Engineering, LLC.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
from matrix_interleaver import matrix_interleaver

class qa_matrix_interleaver(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_interleave(self):
        tb = self.tb

        # set up fg
        cols, rows = 4, 10
        vec = sum((cols * [x,] for x in range(rows)), [])
        expected = cols * list(range(rows))

        src = blocks.vector_source_f(vec, False)
        itlv = matrix_interleaver(gr.sizeof_float, rows=rows, cols=cols)
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
        vec = sum((rows * [x,] for x in range(cols)), [])
        expected = rows * list(range(cols))

        src = blocks.vector_source_f(vec, False)
        itlv = matrix_interleaver(gr.sizeof_float, rows=rows, cols=cols, deint=True)
        snk = blocks.vector_sink_f()

        tb.connect(src, itlv, snk)
        tb.run()
        result = snk.data()

        # check data
        self.assertFloatTuplesAlmostEqual(expected, result)

if __name__ == '__main__':
    gr_unittest.run(qa_matrix_interleaver)
