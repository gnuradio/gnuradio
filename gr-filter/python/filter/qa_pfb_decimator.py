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

from gnuradio import gr, gr_unittest, filter, blocks
import math

def sig_source_c(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.cos(2.*math.pi*freq*x) + \
                1j*math.sin(2.*math.pi*freq*x), t)
    return y

def run_test(tb, channel):
        N = 1000         # number of samples to use
        M = 5            # Number of channels
        fs = 5000.0      # baseband sampling rate
        ifs = M*fs       # input samp rate to decimator

        taps = filter.firdes.low_pass_2(1, ifs, fs/2, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        signals = list()
        add = blocks.add_cc()
        freqs = [-230., 121., 110., -513., 203.]
        Mch = ((len(freqs)-1)/2 + channel) % len(freqs)
        for i in xrange(len(freqs)):
            f = freqs[i] + (M/2-M+i+1)*fs
            data = sig_source_c(ifs, f, 1, N)
            signals.append(blocks.vector_source_c(data))
            tb.connect(signals[i], (add,i))

        s2ss = blocks.stream_to_streams(gr.sizeof_gr_complex, M)
        pfb = filter.pfb_decimator_ccf(M, taps, channel)
        snk = blocks.vector_sink_c()

        tb.connect(add, s2ss)
        for i in xrange(M):
            tb.connect((s2ss,i), (pfb,i))
        tb.connect(pfb, snk)
        tb.run() 

        L = len(snk.data())

        # Each channel is rotated by 2pi/M
        phase = -2*math.pi * channel / M

        # Filter delay is the normal delay of each arm
        tpf = math.ceil(len(taps) / float(M))
        delay = -(tpf - 1.0) / 2.0
        delay = int(delay)

        # Create a time scale that's delayed to match the filter delay
        t = map(lambda x: float(x)/fs, xrange(delay, L+delay))

        # Create known data as complex sinusoids for the baseband freq
        # of the extracted channel is due to decimator output order.
        expected_data = map(lambda x: math.cos(2.*math.pi*freqs[Mch]*x+phase) + \
                                1j*math.sin(2.*math.pi*freqs[Mch]*x+phase), t)
        dst_data = snk.data()

        return (dst_data, expected_data)

class test_pfb_decimator(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        Ntest = 50
        dst_data, expected_data = run_test(self.tb, 0)
        
        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

    def test_001(self):
        Ntest = 50
        dst_data, expected_data = run_test(self.tb, 1)
        
        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

    def test_002(self):
        Ntest = 50
        dst_data, expected_data = run_test(self.tb, 2)
        
        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

    def test_003(self):
        Ntest = 50
        dst_data, expected_data = run_test(self.tb, 3)
        
        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

    def test_004(self):
        Ntest = 50
        dst_data, expected_data = run_test(self.tb, 4)
        
        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

if __name__ == '__main__':
    gr_unittest.run(test_pfb_decimator, "test_pfb_decimator.xml")
