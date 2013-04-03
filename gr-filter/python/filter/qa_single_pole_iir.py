#!/usr/bin/env python
#
# Copyright 2005,2007,2010,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, filter, blocks

class test_single_pole_iir_filter(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_ff_001(self):
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = src_data
        src = blocks.vector_source_f(src_data)
        op = filter.single_pole_iir_filter_ff(1.0)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_ff_002(self):
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = (0, 125, 359.375, 689.453125, 1103.271484, 1590.36255)
        src = blocks.vector_source_f(src_data)
        op = filter.single_pole_iir_filter_ff(0.125)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 3)

    def test_ff_003(self):
        block_size = 2
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = (0, 125, 250, 484.375, 718.75, 1048.828125)
        src = blocks.vector_source_f(src_data)
        s2p = blocks.stream_to_vector(gr.sizeof_float, block_size)
        op = filter.single_pole_iir_filter_ff (0.125, block_size)
        p2s = blocks.vector_to_stream(gr.sizeof_float, block_size)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, s2p, op, p2s, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 3)

    def test_cc_001(self):
        src_data = (0+0j, 1000+1000j, 2000+2000j, 3000+3000j, 4000+4000j, 5000+5000j)
        expected_result = src_data
        src = blocks.vector_source_c(src_data)
        op = filter.single_pole_iir_filter_cc(1.0)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data)

    def test_cc_002(self):
        src_data = (complex(0,0), complex(1000,-1000), complex(2000,-2000),
                    complex(3000,-3000), complex(4000,-4000), complex(5000,-5000))
        expected_result = (complex(0,0), complex(125,-125), complex(359.375,-359.375),
                           complex(689.453125,-689.453125), complex(1103.271484,-1103.271484),
                           complex(1590.36255,-1590.36255))
        src = blocks.vector_source_c(src_data)
        op = filter.single_pole_iir_filter_cc(0.125)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 3)

    def test_cc_003(self):
        block_size = 2
        src_data = (complex(0,0), complex(1000,-1000), complex(2000,-2000),
                    complex(3000,-3000), complex(4000,-4000), complex(5000,-5000))
        expected_result = (complex(0,0), complex(125,-125), complex(250,-250),
                           complex(484.375,-484.375), complex(718.75,-718.75),
                           complex(1048.828125,-1048.828125))
        src = blocks.vector_source_c(src_data)
        s2p = blocks.stream_to_vector(gr.sizeof_gr_complex, block_size)
        op = filter.single_pole_iir_filter_cc(0.125, block_size)
        p2s = blocks.vector_to_stream(gr.sizeof_gr_complex, block_size)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, s2p, op, p2s, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 3)

if __name__ == '__main__':
    gr_unittest.run(test_single_pole_iir_filter, "test_single_pole_iir_filter.xml")

