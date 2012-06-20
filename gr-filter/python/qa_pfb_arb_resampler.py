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

class test_pfb_arb_resampler(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_fff_000(self):
        N = 1000         # number of samples to use
        fs = 1000        # baseband sampling rate
        rrate = 1.123    # resampling rate

        nfilts = 32
        taps = filter.firdes.low_pass_2(nfilts, nfilts*fs, fs/2, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        freq = 100
        signal = gr.sig_source_f(fs, gr.GR_SIN_WAVE, freq, 1)
        head = gr.head(gr.sizeof_float, N)
        pfb = filter.pfb_arb_resampler_fff(rrate, taps)
        snk = gr.vector_sink_f()

        self.tb.connect(signal, head, pfb, snk)
        self.tb.run() 

        Ntest = 50
        L = len(snk.data())
        t = map(lambda x: float(x)/(fs*rrate), xrange(L))

        phase = 0.53013
        expected_data = map(lambda x: math.sin(2.*math.pi*freq*x+phase), t)

        dst_data = snk.data()
        self.assertFloatTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 3)

    def test_ccf_000(self):
        N = 1000         # number of samples to use
        fs = 1000        # baseband sampling rate
        rrate = 1.123    # resampling rate

        nfilts = 32
        taps = filter.firdes.low_pass_2(nfilts, nfilts*fs, fs/2, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        freq = 100
        signal = gr.sig_source_c(fs, gr.GR_SIN_WAVE, freq, 1)
        head = gr.head(gr.sizeof_gr_complex, N)
        pfb = filter.pfb_arb_resampler_ccf(rrate, taps)
        snk = gr.vector_sink_c()

        self.tb.connect(signal, head, pfb, snk)
        self.tb.run() 

        Ntest = 50
        L = len(snk.data())
        t = map(lambda x: float(x)/(fs*rrate), xrange(L))

        phase = 0.53013
        expected_data = map(lambda x: math.cos(2.*math.pi*freq*x+phase) + \
                                1j*math.sin(2.*math.pi*freq*x+phase), t)

        dst_data = snk.data()
        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 3)

if __name__ == '__main__':
    gr_unittest.run(test_pfb_arb_resampler, "test_pfb_arb_resampler.xml")
