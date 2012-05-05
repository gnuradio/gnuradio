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
        expected_data = [0.5, 1.5, 2.5, 3.5]
        src = gr.vector_source_f(src_data)
        op  = filter.fir_filter_fff(1, [0.5, 0.5])
        dst = gr.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccf_001(self):
        src_data = [1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = [0.5+0.5j, 1.5+1.5j, 2.5+2.5j, 3.5+3.5j]
        src = gr.vector_source_c(src_data)
        op  = filter.fir_filter_ccf(1, [0.5, 0.5])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

    def test_fir_filter_ccc_001(self):
        src_data = [1+1j, 2+2j, 3+3j, 4+4j]
        expected_data = [-0.5+1.5j, -1.5+4.5j, -2.5+7.5j, -3.5+10.5j]
        src = gr.vector_source_c(src_data)
        op  = filter.fir_filter_ccc(1, [0.5+1j, 0.5+1j])
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)


    def test_fir_filter_ccc_002(self):
        src_data = 10*[1+1j, 2+2j, 3+3j, 4+4j]
        
        # results derived from original gr.fir_filter_ccc
        expected_data = ((7.537424837948042e-20+7.537424837948042e-20j), (9.131923434324563e-05+9.131923434324563e-05j), (0.0003317668742965907+0.0003317668742965907j), (0.0007230418268591166+0.0007230418268591166j), (0.0012087896466255188+0.0012087896466255188j), (0.0013292605290189385+0.0013292605290189385j), (0.001120875240303576+0.001120875240303576j), (0.000744672492146492+0.000744672492146492j), (0.000429437990533188+0.000429437990533188j), (2.283908543176949e-05+2.283908543176949e-05j), (-0.0002245186478830874-0.0002245186478830874j), (-0.0001157080550910905-0.0001157080550910905j), (0.00041409023106098175+0.00041409023106098175j), (0.0009017843985930085+0.0009017843985930085j), (0.0012520025484263897+0.0012520025484263897j), (0.0014116164529696107+0.0014116164529696107j), (0.001393353333696723+0.001393353333696723j), (0.000912194955162704+0.000912194955162704j), (0.00022649182938039303+0.00022649182938039303j), (-0.00031363096786662936-0.00031363096786662936j), (-0.0003966730728279799-0.0003966730728279799j), (-0.00023757052258588374-0.00023757052258588374j), (0.00021952332463115454+0.00021952332463115454j), (0.0009092430118471384+0.0009092430118471384j), (0.001662317430600524+0.001662317430600524j), (0.0019024648936465383+0.0019024648936465383j), (0.0015955769922584295+0.0015955769922584295j), (0.0009144138311967254+0.0009144138311967254j), (0.0001872836146503687+0.0001872836146503687j), (-0.000581968342885375-0.000581968342885375j), (-0.0009886166080832481-0.0009886166080832481j), (-0.0007480768254026771-0.0007480768254026771j), (0.00018211957649327815+0.00018211957649327815j), (0.0012042406015098095+0.0012042406015098095j), (0.0020200139842927456+0.0020200139842927456j), (0.0023816542234271765+0.0023816542234271765j), (0.002195809967815876+0.002195809967815876j), (0.0012113333214074373+0.0012113333214074373j), (-0.00014088614261709154-0.00014088614261709154j), (-0.0012574587017297745-0.0012574587017297745j))

        taps = filter.firdes.low_pass(1, 1, 0.1, 0.01)
        src = gr.vector_source_c(src_data)
        op  = filter.fir_filter_ccc(1, taps)
        dst = gr.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_data, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_filter, "test_filter.xml")

