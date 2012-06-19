#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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
import blocks_swig

class test_type_conversions(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_char_to_float_identity(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (1.0, 2.0, 3.0, 4.0, 5.0)
        src = gr.vector_source_b(src_data)
        op = blocks_swig.char_to_float()
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

    def test_char_to_float_scale(self):
        src_data = (1, 2, 3, 4, 5)
        expected_data = (0.5, 1.0, 1.5, 2.0, 2.5)
        src = gr.vector_source_b(src_data)
        op = blocks_swig.char_to_float(scale=2.0)
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(expected_data, dst.data())

if __name__ == '__main__':
    gr_unittest.run(test_type_conversions, "test_type_conversions.xml")
