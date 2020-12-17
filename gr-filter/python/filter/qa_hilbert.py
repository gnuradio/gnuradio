#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, fft, filter, blocks
import math


def sig_source_f(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.sin(2. * math.pi * freq * x) for x in t]
    return y


def fir_filter(x, taps):
    y = []
    x2 = (len(taps) - 1) * [0, ] + x
    delay = (len(taps) - 1) // 2
    for i in range(len(x)):
        yi = 0
        for j in range(len(taps)):
            yi += taps[len(taps) - 1 - j] * x2[i + j]
        y.append(complex(x2[i + delay], yi))
    return y


class test_hilbert(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_hilbert(self):
        tb = self.tb
        ntaps = 51
        sampling_freq = 100

        N = int(ntaps + sampling_freq * 0.10)
        data = sig_source_f(sampling_freq, sampling_freq * 0.10, 1.0, N)
        src1 = blocks.vector_source_f(data)

        taps = filter.firdes.hilbert(ntaps, fft.window.WIN_HAMMING)
        expected_result = fir_filter(data, taps)

        hilb = filter.hilbert_fc(ntaps)
        dst1 = blocks.vector_sink_c()
        tb.connect(src1, hilb)
        tb.connect(hilb, dst1)
        tb.run()
        dst_data = dst1.data()

        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_hilbert)
