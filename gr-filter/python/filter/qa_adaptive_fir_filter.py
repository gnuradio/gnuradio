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

class test_adaptive_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_adaptive_fir_filter_ccf_001(self):
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = ((0.5+0.5j), (1.5+1.5j), (3+3j), (5+5j), (5.5+5.5j),
                         (6.5+6.5j), (8+8j), (10+10j), (10.5+10.5j), (11.5+11.5j),
                         (13+13j), (15+15j), (15.5+15.5j), (16.5+16.5j), (18+18j),
                         (20+20j), (20.5+20.5j), (21.5+21.5j), (23+23j), (25+25j),
                         (25.5+25.5j), (26.5+26.5j), (28+28j), (30+30j), (30.5+30.5j),
                         (31.5+31.5j), (33+33j), (35+35j), (35.5+35.5j), (36.5+36.5j),
                         (38+38j), (40+40j), (40.5+40.5j), (41.5+41.5j), (43+43j),
                         (45+45j), (45.5+45.5j), (46.5+46.5j), (48+48j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j))

        src = gr.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccf("test", 1, 20*[0.5, 0.5])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_adaptive_fir_filter_ccf_002(self):
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = ((0.5+0.5j), (5.5+5.5j), (10.5+10.5j), (15.5+15.5j),
                         (20.5+20.5j), (25.5+25.5j), (30.5+30.5j), (35.5+35.5j),
                         (40.5+40.5j), (45.5+45.5j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j), (50+50j),
                         (50+50j), (50+50j), (50+50j), (50+50j))

        src = gr.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccf("test", 4, 20*[0.5, 0.5])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_adaptive_fir_filter_ccc_001(self):
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = ((-0.5+1.5j), (-1.5+4.5j), (-3+9j), (-5+15j),
                         (-5.5+16.5j), (-6.5+19.5j), (-8+24j), (-10+30j),
                         (-10.5+31.5j), (-11.5+34.5j), (-13+39j), (-15+45j),
                         (-15.5+46.5j), (-16.5+49.5j), (-18+54j), (-20+60j),
                         (-20.5+61.5j), (-21.5+64.5j), (-23+69j), (-25+75j),
                         (-25.5+76.5j), (-26.5+79.5j), (-28+84j), (-30+90j),
                         (-30.5+91.5j), (-31.5+94.5j), (-33+99j), (-35+105j),
                         (-35.5+106.5j), (-36.5+109.5j), (-38+114j), (-40+120j),
                         (-40.5+121.5j), (-41.5+124.5j), (-43+129j), (-45+135j),
                         (-45.5+136.5j), (-46.5+139.5j), (-48+144j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j), (-50+150j))
        src = gr.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccc("test", 1, 20*[0.5+1j, 0.5+1j])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_adaptive_fir_filter_ccc_002(self):
        src_data = 40*[1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = ((-0.5+1.5j), (-5.5+16.5j), (-10.5+31.5j),
                         (-15.5+46.5j), (-20.5+61.5j), (-25.5+76.5j),
                         (-30.5+91.5j), (-35.5+106.5j), (-40.5+121.5j),
                         (-45.5+136.5j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j), (-50+150j),
                         (-50+150j), (-50+150j), (-50+150j))
        src = gr.vector_source_c(src_data)
        op  = filter.adaptive_fir_ccc("test", 4, 20*[0.5+1j, 0.5+1j])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_adaptive_filter, "test_adaptive_filter.xml")

