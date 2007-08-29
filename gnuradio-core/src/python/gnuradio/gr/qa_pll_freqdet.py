#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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
        self.fg = gr.flow_graph()

    def tearDown (self):
        self.fg = None

    def test_pll_refout (self):
        expected_result = (1.1489677586e-07,
                           0.972821060568,
                           2.74556447638,
                           5.14063078448,
                           8.00965819311,
                           11.2291393027,
                           14.6967068752,
                           18.3279143967,
                           22.0534772463,
                           25.8170093072,
                           29.5729107661,
                           33.284774699,
                           36.923857393,
                           40.4367950308,
                           43.8452195091,
                           47.1363835133,
                           50.3011949468,
                           53.3336447847,
                           56.2301489564,
                           58.9891659262,
                           61.6107668417,
                           64.0962975824,
                           66.4481356707,
                           68.6694531128,
                           70.7640326003,
                           72.7048735417,
                           74.5033180826,
                           76.2012544926,
                           77.8019199967,
                           79.3088126954,
                           80.7255907715,
                           82.0560369166,
                           83.3039516093,
                           84.47312347,
                           85.5673411194,
                           86.5902864563,
                           87.5456117346,
                           88.4368565575,
                           89.2363918613,
                           89.9860999864,
                           90.688880206,
                           91.3474598523,
                           91.9644654653,
                           92.5423042123,
                           93.0832706099,
                           93.5894872344,
                           94.0629225081,
                           94.5054203452,
                           94.9186882929,
                           95.3043331057,
                           95.6326268597,
                           95.9117515522,
                           96.1801447842,
                           96.437391527,
                           96.6831953314,
                           96.9173605408,
                           97.1397982206,
                           97.3504727968,
                           97.5493842694,
                           97.7366275022,
                           97.9123092169,
                           98.0766013539,
                           98.2297054988,
                           98.3408087235,
                           98.448722155,
                           98.5534457933,
                           98.6549322065,
                           98.7531932527,
                           98.8481459259,
                           98.9397487233,
                           99.0279067813,
                           99.1125074491,
                           99.193438076,
                           99.2705800823,
                           99.3438030304,
                           99.3817663128,
                           99.3911400359,
                           99.4089388448,
                           99.4334136894,
                           99.4630408207,
                           99.4964684305,
                           99.5325166512,
                           99.5701538394,
                           99.6084432158,
                           99.6466021546,
                           99.6839073198,
                           99.7197895289,
                           99.7537270313,
                           99.7542606398,
                           99.7595848672,
                           99.7691186729,
                           99.7822928746,
                           99.7986331535,
                           99.8175940432,
                           99.838713083,
                           99.8614922382,
                           99.8854571901,
                           99.9101454781,
                           99.9351302152,
                           99.9599845147)

        sampling_freq = 10e3
        freq = sampling_freq / 100

        alpha = 0.2
        beta = alpha * alpha / 4.0
        maxf = 1
        minf = -1

        src = gr.sig_source_c (sampling_freq, gr.GR_COS_WAVE, freq, 1.0)
        pll = gr.pll_freqdet_cf(alpha, beta, maxf, minf)
        head = gr.head (gr.sizeof_float, int (freq))
        dst = gr.vector_sink_f ()

        self.fg.connect (src, pll, head)
        self.fg.connect (head, dst)

        self.fg.run ()
        dst_data = dst.data ()

        # convert it from normalized frequency to absolute frequency (Hz)
        dst_data = [i*(sampling_freq/(2*math.pi)) for i in dst_data]

        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 3)

if __name__ == '__main__':
    gr_unittest.main ()
