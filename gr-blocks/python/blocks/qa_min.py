#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
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

class test_min(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = (0, 0.2, -0.25, 0, 12, 0)
        expected_result = (float(min(src_data)),)

        src = blocks.vector_source_f(src_data)
        s2v = blocks.stream_to_vector(gr.sizeof_float, len(src_data))
        op = blocks.min_ff(len(src_data))
        dst = blocks.vector_sink_f()

        self.tb.connect(src, s2v, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def stest_002(self):
	src_data=(-100,-99,-98,-97,-96,-1)
	expected_result = (float(min(src_data)),)

	src = blocks.vector_source_f(src_data)
	s2v = blocks.stream_to_vector(gr.sizeof_float, len(src_data))
	op = blocks.min_ff(len(src_data))
	dst = blocks.vector_sink_f()

	self.tb.connect(src, s2v, op, dst)
	self.tb.run()
	result_data = dst.data()
	self.assertEqual(expected_result, result_data)

    def stest_003(self):
        src_data0 = (0, 2, -3, 0, 12, 0)
        src_data1 = (1, 1,  1, 1,  1, 1)

        expected_result = [float(min(x,y)) for x,y in zip(src_data0, src_data1)]

        src0 = blocks.vector_source_f(src_data0)
        src1 = blocks.vector_source_f(src_data1)
        op = blocks.min_ff(1)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def stest_004(self):
        dim = 2
        src_data0 = (0, 2, -3, 0, 12, 0)
        src_data1 = (1, 1,  1, 1,  1, 1)

        expected_data = []
        tmp = [float(min(x,y)) for x,y in zip(src_data0, src_data1)]
        for i in xrange(len(tmp)/dim):
            expected_data.append(float(min(tmp[i*dim:(i+1)*dim])))

        src0 = blocks.vector_source_f(src_data0)
        s2v0 = blocks.stream_to_vector(gr.sizeof_float,dim)
        src1 = blocks.vector_source_f(src_data1)
        s2v1 = blocks.stream_to_vector(gr.sizeof_float,dim)
        op = blocks.min_ff(dim)
        dst = blocks.vector_sink_f()

        self.tb.connect(src0, s2v0, (op, 0))
        self.tb.connect(src1, s2v1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)


    def stest_s001(self):
        src_data = (0, 2, -3, 0, 12, 0)
        expected_result = (min(src_data),)

        src = blocks.vector_source_s(src_data)
        s2v = blocks.stream_to_vector(gr.sizeof_short,len(src_data))
        op = blocks.min_ss(len(src_data))
        dst = blocks.vector_sink_s()

        self.tb.connect(src, s2v, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def stest_s002(self):
	src_data=(-100,-99,-98,-97,-96,-1)
	expected_result = (min(src_data),)

	src = blocks.vector_source_s(src_data)
	s2v = blocks.stream_to_vector(gr.sizeof_short, len(src_data))
	op = blocks.min_ss(len(src_data))
	dst = blocks.vector_sink_s()

	self.tb.connect(src, s2v, op, dst)
	self.tb.run()
	result_data = dst.data()
	self.assertEqual(expected_result, result_data)


    def stest_s003(self):
        src_data0 = (0, 2, -3, 0, 12, 0)
        src_data1 = (1, 1,  1, 1,  1, 1)

        expected_result = [min(x,y) for x,y in zip(src_data0, src_data1)]

        src0 = blocks.vector_source_s(src_data0)
        src1 = blocks.vector_source_s(src_data1)
        op = blocks.min_ss(1)
        dst = blocks.vector_sink_s()

        self.tb.connect(src0, (op, 0))
        self.tb.connect(src1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def stest_s004(self):
        dim = 2
        src_data0 = (0, 2, -3, 0, 12, 0)
        src_data1 = (1, 1,  1, 1,  1, 1)

        expected_data = []
        tmp = [min(x,y) for x,y in zip(src_data0, src_data1)]
        for i in xrange(len(tmp)/dim):
            expected_data.append(min(tmp[i*dim:(i+1)*dim]))

        src0 = blocks.vector_source_s(src_data0)
        s2v0 = blocks.stream_to_vector(gr.sizeof_short,dim)
        src1 = blocks.vector_source_s(src_data1)
        s2v1 = blocks.stream_to_vector(gr.sizeof_short,dim)
        op = blocks.min_ss(dim)
        dst = blocks.vector_sink_s()

        self.tb.connect(src0, s2v0, (op, 0))
        self.tb.connect(src1, s2v1, (op, 1))
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_min, "test_min.xml")
