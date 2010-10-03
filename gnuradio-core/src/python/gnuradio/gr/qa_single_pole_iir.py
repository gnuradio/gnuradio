#!/usr/bin/env python
#
# Copyright 2005,2007,2010 Free Software Foundation, Inc.
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

class test_single_pole_iir(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = src_data
        src = gr.vector_source_f(src_data)
        op = gr.single_pole_iir_filter_ff (1.0)
        dst = gr.vector_sink_f()
        self.tb.connect (src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data)

    def test_002(self):
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = (0, 125, 359.375, 689.453125, 1103.271484, 1590.36255)
        src = gr.vector_source_f(src_data)
        op = gr.single_pole_iir_filter_ff (0.125)
        dst = gr.vector_sink_f()
        self.tb.connect (src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 3)

    def test_003(self):
        block_size = 2
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = (0, 125, 250, 484.375, 718.75, 1048.828125)
        src = gr.vector_source_f(src_data)
        s2p = gr.serial_to_parallel(gr.sizeof_float, block_size)
        op = gr.single_pole_iir_filter_ff (0.125, block_size)
        p2s = gr.parallel_to_serial(gr.sizeof_float, block_size)
        dst = gr.vector_sink_f()
        self.tb.connect (src, s2p, op, p2s, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual (expected_result, result_data, 3)


if __name__ == '__main__':
    gr_unittest.run(test_single_pole_iir, "test_single_pole_iir.xml")
        
