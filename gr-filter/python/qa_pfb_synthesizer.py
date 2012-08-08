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

class test_pfb_synthesizer(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 10000         # number of samples to use
        M = 5            # Number of channels
        fs = 1000        # baseband sampling rate
        ofs = M*fs       # input samp rate to decimator

        taps = filter.firdes.low_pass_2(M, ofs, fs/2, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        signals = list()
        freqs = [0, 100, 200, -200, -100]
        for i in xrange(len(freqs)):
            signals.append(gr.sig_source_c(fs, gr.GR_SIN_WAVE, freqs[i], 1))

        head = gr.head(gr.sizeof_gr_complex, N)
        pfb = filter.pfb_synthesizer_ccf(M, taps)
        snk = gr.vector_sink_c()

        for i in xrange(M):
            self.tb.connect(signals[i], (pfb,i))

        self.tb.connect(pfb, head, snk)

        self.tb.run() 

        Ntest = 1000
        L = len(snk.data())
        t = map(lambda x: float(x)/ofs, xrange(L))

        # Create known data as sum of complex sinusoids at freqs
        # of the output channels.
        freqs = [-2200, -1100, 0, 1100, 2200]
        expected_data = len(t)*[0,]
        for i in xrange(len(t)):
            expected_data[i] = math.cos(2.*math.pi*freqs[0]*t[i]) + \
                            1j*math.sin(2.*math.pi*freqs[0]*t[i]) + \
                               math.cos(2.*math.pi*freqs[1]*t[i]) + \
                            1j*math.sin(2.*math.pi*freqs[1]*t[i]) + \
                               math.cos(2.*math.pi*freqs[2]*t[i]) + \
                            1j*math.sin(2.*math.pi*freqs[2]*t[i]) + \
                               math.cos(2.*math.pi*freqs[3]*t[i]) + \
                            1j*math.sin(2.*math.pi*freqs[3]*t[i]) + \
                               math.cos(2.*math.pi*freqs[4]*t[i]) + \
                            1j*math.sin(2.*math.pi*freqs[4]*t[i])
        dst_data = snk.data()

        offset = 25
        self.assertComplexTuplesAlmostEqual(expected_data[2000-offset:2000-offset+Ntest],
                                            dst_data[2000:2000+Ntest], 4)

if __name__ == '__main__':
    gr_unittest.run(test_pfb_synthesizer, "test_pfb_synthesizer.xml")
