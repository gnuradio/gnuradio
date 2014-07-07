#!/usr/bin/env python
#
# Copyright 2007,2010,2012,2013 Free Software Foundation, Inc.
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

def sig_source_f(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.sin(2.*math.pi*freq*x), t)
    return y

def sig_source_c(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.cos(2.*math.pi*freq*x) + \
                1j*math.sin(2.*math.pi*freq*x), t)
    return y

class test_fractional_resampler(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_ff(self):
        N = 10000        # number of samples to use
        fs = 1000        # baseband sampling rate
        rrate = 1.123    # resampling rate

        freq = 10
        data = sig_source_f(fs, freq, 1, N)
        signal = blocks.vector_source_f(data)
        op = filter.fractional_interpolator_ff(0, rrate)
        snk = blocks.vector_sink_f()

        self.tb.connect(signal, op, snk)
        self.tb.run()

        Ntest = 5000
        L = len(snk.data())
        t = map(lambda x: float(x)/(fs/rrate), xrange(L))

        phase = 0.1884
        expected_data = map(lambda x: math.sin(2.*math.pi*freq*x+phase), t)

        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 3)


    def test_002_cc(self):
        N = 10000        # number of samples to use
        fs = 1000        # baseband sampling rate
        rrate = 1.123    # resampling rate

        freq = 10
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        op = filter.fractional_interpolator_cc(0.0, rrate)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, op, snk)
        self.tb.run()

        Ntest = 5000
        L = len(snk.data())
        t = map(lambda x: float(x)/(fs/rrate), xrange(L))

        phase = 0.1884
        expected_data = map(lambda x: math.cos(2.*math.pi*freq*x+phase) + \
                                1j*math.sin(2.*math.pi*freq*x+phase), t)

        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 3)


if __name__ == '__main__':
    gr_unittest.run(test_fractional_resampler, "test_fractional_resampler.xml")
