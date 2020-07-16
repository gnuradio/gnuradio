#!/usr/bin/env python
#
# Copyright 2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, filter, blocks

import math

def sig_source_f(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.sin(2.*math.pi*freq*x) for x in t]
    return y

def sig_source_c(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.cos(2.*math.pi*freq*x) + \
                1j*math.sin(2.*math.pi*freq*x) for x in t]
    return y

class test_mmse_resampler(gr_unittest.TestCase):

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
        op = filter.mmse_interpolator_ff(0, rrate)
        snk = blocks.vector_sink_f()

        self.tb.connect(signal, op, snk)
        self.tb.run()

        Ntest = 5000
        L = len(snk.data())
        t = [float(x) / (fs / rrate) for x in range(L)]

        phase = 0.1884
        expected_data = [math.sin(2.*math.pi*freq*x+phase) for x in t]

        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 3)


    def test_002_cc(self):
        N = 10000        # number of samples to use
        fs = 1000        # baseband sampling rate
        rrate = 1.123    # resampling rate

        freq = 10
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        op = filter.mmse_interpolator_cc(0.0, rrate)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, op, snk)
        self.tb.run()

        Ntest = 5000
        L = len(snk.data())
        t = [float(x) / (fs / rrate) for x in range(L)]

        phase = 0.1884
        expected_data = [math.cos(2.*math.pi*freq*x+phase) + \
                                1j*math.sin(2.*math.pi*freq*x+phase) for x in t]

        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(expected_data[-Ntest:], dst_data[-Ntest:], 3)


if __name__ == '__main__':
    gr_unittest.run(test_mmse_resampler, "test_mmse_resampler.xml")
