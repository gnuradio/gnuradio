#!/usr/bin/env python
#
# Copyright 2011,2012 Free Software Foundation, Inc.
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

class test_short_to_char (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):

        src_data = range(0, 32767, 32767/127)
        src_data = [int(s) for s in src_data]
        expected_result = range(0, 128)
        src = gr.vector_source_s(src_data)
        op = gr.short_to_char()
        dst = gr.vector_sink_b()

        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

    def test_002(self):

        vlen = 3
        src_data = range(0, 32400, 32767/127)
        src_data = [int(s) for s in src_data]
        expected_result = range(0, 126)
        src = gr.vector_source_s(src_data)
        s2v = gr.stream_to_vector(gr.sizeof_short, vlen)
        op = gr.short_to_char(vlen)
        v2s = gr.vector_to_stream(gr.sizeof_char, vlen)
        dst = gr.vector_sink_b()

        self.tb.connect(src, s2v, op, v2s, dst)
        self.tb.run()
        result_data = list(dst.data())

        self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_short_to_char, "test_short_to_char.xml")

