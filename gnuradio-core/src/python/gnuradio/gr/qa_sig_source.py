#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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

class test_sig_source (gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.flow_graph ()

    def tearDown (self):
        self.fg = None

    def test_const_f (self):
        fg = self.fg
        expected_result = (1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5)
        src1 = gr.sig_source_f (1e6, gr.GR_CONST_WAVE, 0, 1.5)
        op = gr.head (gr.sizeof_float, 10)
        dst1 = gr.vector_sink_f ()
        fg.connect (src1, op)
        fg.connect (op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
    
    def test_const_i (self):
        fg = self.fg
        expected_result = (1, 1, 1, 1)
        src1 = gr.sig_source_i (1e6, gr.GR_CONST_WAVE, 0, 1)
        op = gr.head (gr.sizeof_int, 4)
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op)
        fg.connect (op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
    
    def test_sine_f (self):
        fg = self.fg
        sqrt2 = math.sqrt(2) / 2
        expected_result = (0, sqrt2, 1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0)
        src1 = gr.sig_source_f (8, gr.GR_SIN_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        fg.connect (src1, op)
        fg.connect (op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 5)

    def test_cosine_f (self):
        fg = self.fg
        sqrt2 = math.sqrt(2) / 2
        expected_result = (1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0, sqrt2, 1)
        src1 = gr.sig_source_f (8, gr.GR_COS_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        fg.connect (src1, op)
        fg.connect (op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.main ()
