#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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
import blocks_swig as blocks
import math

def sig_source_c(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.cos(2.*math.pi*freq*x) + \
                1j*math.sin(2.*math.pi*freq*x), t)
    return y

class test_pfb_decimator(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        M = 5            # Number of channels
        fs = 1000        # baseband sampling rate
        ifs = M*fs       # input samp rate to decimator
        channel = 0      # Extract channel 0

        taps = filter.firdes.low_pass_2(1, ifs, fs/2, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        signals = list()
        add = blocks.add_cc()
        freqs = [-200, -100, 0, 100, 200]
        for i in xrange(len(freqs)):
            f = freqs[i] + (M/2-M+i+1)*fs
            data = sig_source_c(ifs, f, 1, N)
            signals.append(blocks.vector_source_c(data))
            self.tb.connect(signals[i], (add,i))

        head = blocks.head(gr.sizeof_gr_complex, N)
        s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, M)
        pfb = filter.pfb_decimator_ccf(M, taps, channel)
        snk = blocks.vector_sink_c()

        self.tb.connect(add, head, s2ss)
        for i in xrange(M):
            self.tb.connect((s2ss,i), (pfb,i))
        self.tb.connect(pfb, snk)

        self.tb.run() 

        Ntest = 50
        L = len(snk.data())
        t = map(lambda x: float(x)/fs, xrange(L))

        # Create known data as complex sinusoids for the baseband freq
        # of the extracted channel is due to decimator output order.
        phase = 0
        expected_data = map(lambda x: math.cos(2.*math.pi*freqs[2]*x+phase) + \
                                1j*math.sin(2.*math.pi*freqs[2]*x+phase), t)

        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

    def test_001(self):
        N = 1000         # number of samples to use
        M = 5            # Number of channels
        fs = 1000        # baseband sampling rate
        ifs = M*fs       # input samp rate to decimator
        channel = 1      # Extract channel 0

        taps = filter.firdes.low_pass_2(1, ifs, fs/2, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        signals = list()
        add = blocks.add_cc()
        freqs = [-200, -100, 0, 100, 200]
        for i in xrange(len(freqs)):
            f = freqs[i] + (M/2-M+i+1)*fs
            data = sig_source_c(ifs, f, 1, N)
            signals.append(blocks.vector_source_c(data))
            self.tb.connect(signals[i], (add,i))

        s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, M)
        pfb = filter.pfb_decimator_ccf(M, taps, channel)
        snk = blocks.vector_sink_c()

        self.tb.connect(add, s2ss)
        for i in xrange(M):
            self.tb.connect((s2ss,i), (pfb,i))
        self.tb.connect(pfb, snk)

        self.tb.run() 

        Ntest = 50
        L = len(snk.data())
        t = map(lambda x: float(x)/fs, xrange(L))

        # Create known data as complex sinusoids for the baseband freq
        # of the extracted channel is due to decimator output order.
        phase = 6.1575
        expected_data = map(lambda x: math.cos(2.*math.pi*freqs[3]*x+phase) + \
                                1j*math.sin(2.*math.pi*freqs[3]*x+phase), t)
        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

if __name__ == '__main__':
    gr_unittest.run(test_pfb_decimator, "test_pfb_decimator.xml")
