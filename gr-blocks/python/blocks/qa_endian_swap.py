#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
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
import ctypes

class test_endian_swap(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [1,2,3,4]
        expected_result = [256, 512, 768, 1024];

        src = blocks.vector_source_s(src_data)
        op = blocks.endian_swap(2)
        dst = blocks.vector_sink_s()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

    def test_002(self):

        src_data = [1,2,3,4]
        expected_result = [16777216, 33554432, 50331648, 67108864];

        src = blocks.vector_source_i(src_data)
        op = blocks.endian_swap(4)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_endian_swap, "test_endian_swap.xml")

