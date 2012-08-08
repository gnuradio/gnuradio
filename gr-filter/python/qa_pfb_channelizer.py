#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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
import filter_swig as filter
import math

class test_pfb_channelizer(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        M = 5            # Number of channels to channelize
        fs = 1000        # baseband sampling rate
        ifs = M*fs       # input samp rate to channelizer

        taps = filter.firdes.low_pass_2(1, ifs, 500, 50,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        signals = list()
        add = gr.add_cc()
        freqs = [-200, -100, 0, 100, 200]
        for i in xrange(len(freqs)):
            f = freqs[i] + (M/2-M+i+1)*fs
            signals.append(gr.sig_source_c(ifs, gr.GR_SIN_WAVE, f, 1))
            self.tb.connect(signals[i], (add,i))

        head = gr.head(gr.sizeof_gr_complex, N)
        s2ss = gr.stream_to_streams(gr.sizeof_gr_complex, M)
        pfb = filter.pfb_channelizer_ccf(M, taps, 1)

        self.tb.connect(add, head, s2ss)

        snks = list()
        for i in xrange(M):
            snks.append(gr.vector_sink_c())
            self.tb.connect((s2ss,i), (pfb,i))
            self.tb.connect((pfb, i), snks[i])

        self.tb.run() 

        Ntest = 50
        L = len(snks[0].data())
        t = map(lambda x: float(x)/fs, xrange(L))

        # Adjusted phase rotations for data
        p0 = 0
        p1 =  math.pi*0.51998885
        p2 = -math.pi*0.96002233
        p3 =  math.pi*0.96002233
        p4 = -math.pi*0.51998885

        # Create known data as complex sinusoids at the different baseband freqs
        # the different channel numbering is due to channelizer output order.
        expected0_data = map(lambda x: math.cos(2.*math.pi*freqs[2]*x+p0) + \
                                       1j*math.sin(2.*math.pi*freqs[2]*x+p0), t)
        expected1_data = map(lambda x: math.cos(2.*math.pi*freqs[3]*x+p1) + \
                                       1j*math.sin(2.*math.pi*freqs[3]*x+p1), t)
        expected2_data = map(lambda x: math.cos(2.*math.pi*freqs[4]*x+p2) + \
                                       1j*math.sin(2.*math.pi*freqs[4]*x+p2), t)
        expected3_data = map(lambda x: math.cos(2.*math.pi*freqs[0]*x+p3) + \
                                       1j*math.sin(2.*math.pi*freqs[0]*x+p3), t)
        expected4_data = map(lambda x: math.cos(2.*math.pi*freqs[1]*x+p4) + \
                                       1j*math.sin(2.*math.pi*freqs[1]*x+p4), t)

        dst0_data = snks[0].data()
        dst1_data = snks[1].data()
        dst2_data = snks[2].data()
        dst3_data = snks[3].data()
        dst4_data = snks[4].data()

        self.assertComplexTuplesAlmostEqual(expected0_data[-Ntest:], dst0_data[-Ntest:], 3)
        self.assertComplexTuplesAlmostEqual(expected1_data[-Ntest:], dst1_data[-Ntest:], 3)
        self.assertComplexTuplesAlmostEqual(expected2_data[-Ntest:], dst2_data[-Ntest:], 3)
        self.assertComplexTuplesAlmostEqual(expected3_data[-Ntest:], dst3_data[-Ntest:], 3)
        self.assertComplexTuplesAlmostEqual(expected4_data[-Ntest:], dst4_data[-Ntest:], 3)

if __name__ == '__main__':
    gr_unittest.run(test_pfb_channelizer, "test_pfb_channelizer.xml")
