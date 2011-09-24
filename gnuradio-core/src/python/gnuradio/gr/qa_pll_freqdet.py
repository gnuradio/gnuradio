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

class test_pll_freqdet (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test_pll_refout (self):
        expected_result = (0.0,
                           1.1489677586e-07,
                           0.972820967928,
                           2.74556447638,
                           5.14063115504,
                           8.00965893424,
                           11.2291407849,
                           14.6967083575,
                           18.3279143967,
                           22.0534772463,
                           25.8170063427,
                           29.5729048372,
                           33.28476877,
                           36.923851464,
                           40.4367920663,
                           43.8452165447,
                           47.1363805488,
                           50.3011890178,
                           53.3336388558,
                           56.2301430274,
                           58.9891659262,
                           61.6107668417,
                           64.0962975824,
                           66.4481415997,
                           68.6694590418,
                           70.7640385293,
                           72.7048794706,
                           74.5033240115,
                           76.2012544926,
                           77.8019140677,
                           79.3088126954,
                           80.7255967005,
                           82.0560428456,
                           83.3039575383,
                           84.473129399,
                           85.5673470484,
                           86.5902864563,
                           87.5456176636,
                           88.4368624865,
                           89.2363977903,
                           89.9861118444,
                           90.6888920639,
                           91.3474657813,
                           91.9644713943,
                           92.5423101413,
                           93.0832765389,
                           93.5894931633,
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
                           97.9123032879,
                           98.0766013539,
                           98.2297054988,
                           98.3408027946,
                           98.4487102971,
                           98.5534280064,
                           98.6549025616,
                           98.7531576788,
                           98.848110352,
                           98.9397131494,
                           99.0278712074,
                           99.1124718752,
                           99.193408431,
                           99.2705445084,
                           99.3437733855,
                           99.3817366678,
                           99.391110391,
                           99.4089151289,
                           99.4333959024,
                           99.4630289627,
                           99.4964565726,
                           99.5325047932,
                           99.5701419814,
                           99.6084313579,
                           99.6465902967,
                           99.6838954618,
                           99.7197776709,
                           99.7537270313,
                           99.7542606398,
                           99.7595848672,
                           99.7691305308,
                           99.7823047325,
                           99.7986450115,
                           99.8176059012,
                           99.838724941,
                           99.8615040962,
                           99.8854690481,
                           99.910157336,
                           99.9351302152)

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

        self.tb.connect (src, pll, head)
        self.tb.connect (head, dst)

        self.tb.run ()
        dst_data = dst.data ()

        # convert it from normalized frequency to absolute frequency (Hz)
        dst_data = [i*(sampling_freq/(2*math.pi)) for i in dst_data]

        self.assertFloatTuplesAlmostEqual (expected_result, dst_data, 3)

if __name__ == '__main__':
    gr_unittest.run(test_pll_freqdet, "test_pll_freqdet.xml")
