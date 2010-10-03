#!/usr/bin/env python
#
# Copyright 2004,2007,2010 Free Software Foundation, Inc.
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

class test_interleave (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_int_001 (self):
        lenx = 64
        src0 = gr.vector_source_f (range (0, lenx, 4))
        src1 = gr.vector_source_f (range (1, lenx, 4))
        src2 = gr.vector_source_f (range (2, lenx, 4))
        src3 = gr.vector_source_f (range (3, lenx, 4))
        op = gr.interleave (gr.sizeof_float)
        dst = gr.vector_sink_f ()

        self.tb.connect (src0, (op, 0))
        self.tb.connect (src1, (op, 1))
        self.tb.connect (src2, (op, 2))
        self.tb.connect (src3, (op, 3))
        self.tb.connect (op, dst)
        self.tb.run ()
        expected_result = tuple (range (lenx))
        result_data = dst.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_deint_001 (self):
        lenx = 64
        src = gr.vector_source_f (range (lenx))
        op = gr.deinterleave (gr.sizeof_float)
        dst0 = gr.vector_sink_f ()
        dst1 = gr.vector_sink_f ()
        dst2 = gr.vector_sink_f ()
        dst3 = gr.vector_sink_f ()

        self.tb.connect (src, op)
        self.tb.connect ((op, 0), dst0)
        self.tb.connect ((op, 1), dst1)
        self.tb.connect ((op, 2), dst2)
        self.tb.connect ((op, 3), dst3)
        self.tb.run ()

        expected_result0 = tuple (range (0, lenx, 4))
        expected_result1 = tuple (range (1, lenx, 4))
        expected_result2 = tuple (range (2, lenx, 4))
        expected_result3 = tuple (range (3, lenx, 4))

        self.assertFloatTuplesAlmostEqual (expected_result0, dst0.data ())
        self.assertFloatTuplesAlmostEqual (expected_result1, dst1.data ())
        self.assertFloatTuplesAlmostEqual (expected_result2, dst2.data ())
        self.assertFloatTuplesAlmostEqual (expected_result3, dst3.data ())

if __name__ == '__main__':
    gr_unittest.run(test_interleave, "test_interleave.xml")
        
