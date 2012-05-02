#!/usr/bin/env python
#
# Copyright 2008,2010,2012 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr, gr_unittest
import filter_swig as filter

class test_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_fir_filter_fff_001(self):
        src_data = [1, 2, 3, 4]
        expected_data = [0, 0.5, 1.5, 2.5]
        src = gr.vector_source_f(src_data)
        op  = filter.fir_filter_fff(1, [0.5, 0.5])
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccf_001(self):
        src_data = [1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = [0+0j, 0.5+0.5j, 1.5+1.5j, 2.5+2.5j]
        src = gr.vector_source_c(src_data)
        op  = filter.fir_filter_ccf(1, [0.5, 0.5])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccc_001(self):
        src_data = [1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = [0+0j, -0.5+1.5j, -1.5+4.5j, -2.5+7.5j]
        src = gr.vector_source_c(src_data)
        op  = filter.fir_filter_ccc(1, [0.5+1j, 0.5+1j])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_filter, "test_filter.xml")

