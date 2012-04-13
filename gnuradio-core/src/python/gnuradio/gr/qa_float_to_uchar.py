#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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
import ctypes

class test_float_to_uchar (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):

        src_data = (0.0, 1.1, 2.2, 3.3, 4.4, 5.5, -1.1, -2.2, -3.3, -4.4, -5.5)
        expected_result = [0, 1, 2, 3, 4, 6, 0, 0, 0, 0, 0]
        src = gr.vector_source_f(src_data)
        op = gr.float_to_uchar()
        dst = gr.vector_sink_b()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

    def test_002(self):

        src_data = ( 254.0,  255.0, 256.0)
        expected_result = [ 254, 255, 255 ]
        src = gr.vector_source_f(src_data)
        op = gr.float_to_uchar()
        dst = gr.vector_sink_b()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_float_to_uchar, "test_float_to_uchar.xml")

