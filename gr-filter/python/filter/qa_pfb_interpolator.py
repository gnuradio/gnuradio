#!/usr/bin/env python
#
# Copyright 2012-2014 Free Software Foundation, Inc.
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

class test_pfb_interpolator(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        M = 5            # Number of channels
        fs = 1000        # baseband sampling rate
        ofs = M*fs       # output samp rate of interpolator

        taps = filter.firdes.low_pass_2(M, ofs, fs/4, fs/10,
                                        attenuation_dB=80,
                                        window=filter.firdes.WIN_BLACKMAN_hARRIS)

        freq = 123.456
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        pfb = filter.pfb_interpolator_ccf(M, taps)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, pfb)
        self.tb.connect(pfb, snk)

        self.tb.run()

        Ntest = 50
        L = len(snk.data())

        # Phase rotation through the filters
        phase = 4.8870112969978994

        # Create a time scale
        t = map(lambda x: float(x)/ofs, xrange(0, L))

        # Create known data as complex sinusoids for the baseband freq
        # of the extracted channel is due to decimator output order.
        expected_data = map(lambda x: math.cos(2.*math.pi*freq*x+phase) + \
                                1j*math.sin(2.*math.pi*freq*x+phase), t)

        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 4)

if __name__ == '__main__':
    gr_unittest.run(test_pfb_interpolator, "test_pfb_interpolator.xml")
