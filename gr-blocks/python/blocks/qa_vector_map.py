#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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
import math

class test_vector_map(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_reversing(self):
        # Chunk data in blocks of N and reverse the block contents.
        N = 5
        src_data = range(0, 20)
        expected_result = []
        for i in range(N-1, len(src_data), N):
            for j in range(0, N):
                expected_result.append(1.0*(i-j))
        mapping = [list(reversed([(0, i) for i in range(0, N)]))]
        src = blocks.vector_source_f(src_data, False, N)
        vmap = blocks.vector_map(gr.sizeof_float, (N, ), mapping)
        dst = blocks.vector_sink_f(N)
        self.tb.connect(src, vmap, dst)
        self.tb.run()
        result_data = list(dst.data())
        self.assertEqual(expected_result, result_data)

    def test_vector_to_streams(self):
        # Split an input vector into N streams.
        N = 5
        M = 20
        src_data = range(0, M)
        expected_results = []
        for n in range(0, N):
            expected_results.append(range(n, M, N))
        mapping = [[(0, n)] for n in range(0, N)]
        src = blocks.vector_source_f(src_data, False, N)
        vmap = blocks.vector_map(gr.sizeof_float, (N, ), mapping)
        dsts = [blocks.vector_sink_f(1) for n in range(0, N)]
        self.tb.connect(src, vmap)
        for n in range(0, N):
            self.tb.connect((vmap, n), dsts[n])
        self.tb.run()
        for n in range(0, N):
            result_data = list(dsts[n].data())
            self.assertEqual(expected_results[n], result_data)

    def test_interleaving(self):
        # Takes 3 streams (a, b and c)
        # Outputs 2 streams.
        # First (d) is interleaving of a and b.
        # Second (e) is interleaving of a and b and c.  c is taken in
        #     chunks of 2 which are reversed.
        A = (1, 2, 3, 4, 5)
        B = (11, 12, 13, 14, 15)
        C = (99, 98, 97, 96, 95, 94, 93, 92, 91, 90)
        expected_D = (1, 11, 2, 12, 3, 13, 4, 14, 5, 15)
        expected_E = (1, 11, 98, 99, 2, 12, 96, 97, 3, 13, 94, 95,
                      4, 14, 92, 93, 5, 15, 90, 91)
        mapping = [[(0, 0), (1, 0)], # mapping to produce D
                   [(0, 0), (1, 0), (2, 1), (2, 0)], # mapping to produce E
                   ]
        srcA = blocks.vector_source_f(A, False, 1)
        srcB = blocks.vector_source_f(B, False, 1)
        srcC = blocks.vector_source_f(C, False, 2)
        vmap =  blocks.vector_map(gr.sizeof_int, (1, 1, 2), mapping)
        dstD = blocks.vector_sink_f(2)
        dstE = blocks.vector_sink_f(4)
        self.tb.connect(srcA, (vmap, 0))
        self.tb.connect(srcB, (vmap, 1))
        self.tb.connect(srcC, (vmap, 2))
        self.tb.connect((vmap, 0), dstD)
        self.tb.connect((vmap, 1), dstE)
        self.tb.run()
        self.assertEqual(expected_D, dstD.data())
        self.assertEqual(expected_E, dstE.data())

if __name__ == '__main__':
    gr_unittest.run(test_vector_map, "test_vector_map.xml")

