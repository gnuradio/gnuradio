#!/usr/bin/env python
#
# Copyright 2005,2007,2010,2012,2013 Free Software Foundation, Inc.
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

def calc_expected_result(src_data, n):
    assert (len(src_data) % n) == 0
    result = [list() for x in range(n)]
    #print "len(result) =", len(result)
    for i in xrange(len(src_data)):
        (result[i % n]).append(src_data[i])
    return [tuple(x) for x in result]


class test_pipe_fittings(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        """
        Test stream_to_streams.
        """
        n = 8
        src_len = n * 8
        src_data = range(src_len)

        expected_results = calc_expected_result(src_data, n)
        #print "expected results: ", expected_results
        src = blocks.vector_source_i(src_data)
        op = blocks.stream_to_streams(gr.sizeof_int, n)
        self.tb.connect(src, op)

        dsts = []
        for i in range(n):
            dst = blocks.vector_sink_i()
            self.tb.connect((op, i), (dst, 0))
            dsts.append(dst)

        self.tb.run()

        for d in range(n):
            self.assertEqual(expected_results[d], dsts[d].data())

    def test_002(self):

        # Test streams_to_stream (using stream_to_streams).

        n = 8
        src_len = n * 8
        src_data = tuple(range(src_len))
        expected_results = src_data

        src = blocks.vector_source_i(src_data)
        op1 = blocks.stream_to_streams(gr.sizeof_int, n)
        op2 = blocks.streams_to_stream(gr.sizeof_int, n)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1)
        for i in range(n):
            self.tb.connect((op1, i), (op2, i))
        self.tb.connect(op2, dst)

        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_003(self):

        #Test streams_to_vector (using stream_to_streams & vector_to_stream).

        n = 8
        src_len = n * 8
        src_data = tuple(range(src_len))
        expected_results = src_data

        src = blocks.vector_source_i(src_data)
        op1 = blocks.stream_to_streams(gr.sizeof_int, n)
        op2 = blocks.streams_to_vector(gr.sizeof_int, n)
        op3 = blocks.vector_to_stream(gr.sizeof_int, n)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1)
        for i in range(n):
            self.tb.connect((op1, i), (op2, i))
        self.tb.connect(op2, op3, dst)

        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_004(self):

        #Test vector_to_streams.

        n = 8
        src_len = n * 8
        src_data = tuple(range(src_len))
        expected_results = src_data

        src = blocks.vector_source_i(src_data)
        op1 = blocks.stream_to_vector(gr.sizeof_int, n)
        op2 = blocks.vector_to_streams(gr.sizeof_int, n)
        op3 = blocks.streams_to_stream(gr.sizeof_int, n)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1, op2)
        for i in range(n):
            self.tb.connect((op2, i), (op3, i))
        self.tb.connect(op3, dst)

        self.tb.run()
        self.assertEqual(expected_results, dst.data())


if __name__ == '__main__':
    gr_unittest.run(test_pipe_fittings, "test_pipe_fittings.xml")
