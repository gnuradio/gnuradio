#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2011 Free Software Foundation, Inc.
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

    def test_pll_freqdet (self):
        expected_result = (0.0,
                           4.33888922882e-08,
                           0.367369994515,
                           1.08135249597,
                           2.10983253908,
                           3.42221529438,
                           4.98940390402,
                           6.78379190842,
                           8.77923286024,
                           10.9510106794,
                           13.2758363182,
                           15.7317829127,
                           18.2982902299,
                           20.9561068599,
                           23.6755271122,
                           26.452952094,
                           29.2731265301,
                           32.1219053479,
                           34.9862418188,
                           37.8540971414,
                           40.7144315483,
                           43.5571390869,
                           46.3730179743,
                           49.1537231663,
                           51.8917218889,
                           54.58026103,
                           57.2015358514,
                           59.7513664199,
                           62.2380533124,
                           64.657612252,
                           67.006640002,
                           69.2822432184,
                           71.4820384499,
                           73.6041047056,
                           75.6469478817,
                           77.6094829742,
                           79.4909866472,
                           81.2911031615,
                           83.0097850853,
                           84.6355598352,
                           86.1820937186,
                           87.6504420946,
                           89.0418441206,
                           90.3577286819,
                           91.5996432431,
                           92.7692775646,
                           93.8684162704,
                           94.8989269904,
                           95.8627662892,
                           96.7619381633,
                           97.598505899,
                           98.362769679,
                           99.0579904444,
                           99.6992633875,
                           100.288805948,
                           100.828805921,
                           101.321421457,
                           101.76878699,
                           102.17300138,
                           102.536116055,
                           102.860158727,
                           103.147085962,
                           103.398830608,
                           103.617254366,
                           103.792467691,
                           103.939387906,
                           104.060030865,
                           104.15631756,
                           104.230085975,
                           104.283067372,
                           104.316933727,
                           104.333238432,
                           104.333440018,
                           104.318914008,
                           104.290941063,
                           104.250742554,
                           104.187634452,
                           104.103822339,
                           104.013227468,
                           103.916810336,
                           103.815448432,
                           103.709936239,
                           103.600997093,
                           103.489283183,
                           103.375351833,
                           103.259712936,
                           103.142828952,
                           103.025091195,
                           102.90686726,
                           102.776726069,
                           102.648078982,
                           102.521459607,
                           102.397294831,
                           102.275999684,
                           102.157882471,
                           102.043215927,
                           101.93218978,
                           101.824958181,
                           101.72159228,
                           101.622151366)

        sampling_freq = 10e3
        freq = sampling_freq / 100

        loop_bw = math.pi/100.0
        maxf = 1
        minf = -1

        src = gr.sig_source_c (sampling_freq, gr.GR_COS_WAVE, freq, 1.0)
        pll = gr.pll_freqdet_cf(loop_bw, maxf, minf)
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
