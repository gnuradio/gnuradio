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

class test_sig_source (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_const_f (self):
        tb = self.tb
        expected_result = (1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5)
        src1 = gr.sig_source_f (1e6, gr.GR_CONST_WAVE, 0, 1.5)
        op = gr.head (gr.sizeof_float, 10)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
    
    def test_const_i (self):
        tb = self.tb
        expected_result = (1, 1, 1, 1)
        src1 = gr.sig_source_i (1e6, gr.GR_CONST_WAVE, 0, 1)
        op = gr.head (gr.sizeof_int, 4)
        dst1 = gr.vector_sink_i ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
    
    def test_sine_f (self):
        tb = self.tb
        sqrt2 = math.sqrt(2) / 2
        expected_result = (0, sqrt2, 1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0)
        src1 = gr.sig_source_f (8, gr.GR_SIN_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 5)

    def test_cosine_f (self):
        tb = self.tb
        sqrt2 = math.sqrt(2) / 2
        expected_result = (1, sqrt2, 0, -sqrt2, -1, -sqrt2, 0, sqrt2, 1)
        src1 = gr.sig_source_f (8, gr.GR_COS_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 5)
        
    def test_sqr_c (self):
        tb = self.tb						#arg6 is a bit before -PI/2
        expected_result = (1j, 1j, 0, 0, 1, 1, 1+0j, 1+1j, 1j)
        src1 = gr.sig_source_c (8, gr.GR_SQR_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_gr_complex, 9)
        dst1 = gr.vector_sink_c ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        
    def test_tri_c (self):
        tb = self.tb
        expected_result = (1+.5j, .75+.75j, .5+1j, .25+.75j, 0+.5j, .25+.25j, .5+0j, .75+.25j, 1+.5j)
        src1 = gr.sig_source_c (8, gr.GR_TRI_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_gr_complex, 9)
        dst1 = gr.vector_sink_c ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertComplexTuplesAlmostEqual (expected_result, dst_data, 5)
        
    def test_saw_c (self):
        tb = self.tb
        expected_result = (.5+.25j, .625+.375j, .75+.5j, .875+.625j, 0+.75j, .125+.875j, .25+1j, .375+.125j, .5+.25j)
        src1 = gr.sig_source_c (8, gr.GR_SAW_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_gr_complex, 9)
        dst1 = gr.vector_sink_c ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertComplexTuplesAlmostEqual (expected_result, dst_data, 5)
    
    def test_sqr_f (self):
        tb = self.tb
        expected_result = (0, 0, 0, 0, 1, 1, 1, 1, 0)
        src1 = gr.sig_source_f (8, gr.GR_SQR_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        
    def test_tri_f (self):
        tb = self.tb
        expected_result = (1, .75, .5, .25, 0, .25, .5, .75, 1)
        src1 = gr.sig_source_f (8, gr.GR_TRI_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 5)
        
    def test_saw_f (self):
        tb = self.tb
        expected_result = (.5, .625, .75, .875, 0, .125, .25, .375, .5)
        src1 = gr.sig_source_f (8, gr.GR_SAW_WAVE, 1.0, 1.0)
        op = gr.head (gr.sizeof_float, 9)
        dst1 = gr.vector_sink_f ()
        tb.connect (src1, op)
        tb.connect (op, dst1)
        tb.run ()
        dst_data = dst1.data ()
        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_sig_source, "test_sig_source.xml")
