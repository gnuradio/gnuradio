#!/usr/bin/env python
#
# Copyright 2007,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
import math


class test_max (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()


    def tearDown (self):
        self.tb = None


    def test_001(self):

        src_data = (0,0.2,-0.3,0,12,0)
        expected_result = (float(max(src_data)), )

        src = gr.vector_source_f(src_data)
        s2v = gr.stream_to_vector(gr.sizeof_float, len(src_data))
        op = gr.max_ff( len(src_data) )
        dst = gr.vector_sink_f()


        self.tb.connect(src, s2v, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_002(self):

	src_data=(-100,-99,-98,-97,-96,-1)
	expected_result = (float(max(src_data)), )

	src = gr.vector_source_f(src_data)
	s2v = gr.stream_to_vector(gr.sizeof_float, len(src_data))
	op = gr.max_ff( len(src_data) )
	dst = gr.vector_sink_f()

	self.tb.connect(src, s2v, op, dst)
	self.tb.run()
	result_data = dst.data()
	self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_max, "test_max.xml")

