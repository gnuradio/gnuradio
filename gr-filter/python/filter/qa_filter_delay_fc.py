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


def sin_source_f(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.sin(2. * math.pi * freq * x) for x in t]
    return y


def cos_source_f(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.cos(2. * math.pi * freq * x) for x in t]
    return y


def fir_filter(x, taps, delay):
    y = []
    x2 = (len(taps) - 1) * [0, ] + x
    for i in range(len(x)):
        yi = 0
        for j in range(len(taps)):
            yi += taps[len(taps) - 1 - j] * x2[i + j]
        y.append(complex(x2[i + delay], yi))
    return y


def fir_filter2(x1, x2, taps, delay):
    y = []
    x1_2 = (len(taps) - 1) * [0, ] + x1
    x2_2 = (len(taps) - 1) * [0, ] + x2
    for i in range(len(x2)):
        yi = 0
        for j in range(len(taps)):
            yi += taps[len(taps) - 1 - j] * x2_2[i + j]
        y.append(complex(x1_2[i + delay], yi))
    return y


class test_filter_delay_fc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_filter_delay_one_input(self):
        tb = self.tb

        sampling_freq = 100

        ntaps = 51
        N = int(ntaps + sampling_freq * 0.10)
        data = sin_source_f(sampling_freq, sampling_freq * 0.10, 1.0, N)
        src1 = blocks.vector_source_f(data)
        dst2 = blocks.vector_sink_c()

        # calculate taps
        taps = filter.firdes.hilbert(ntaps, fft.window.WIN_HAMMING)
        hd = filter.filter_delay_fc(taps)

        expected_result = fir_filter(data, taps, (ntaps - 1) // 2)

        tb.connect(src1, hd)
        tb.connect(hd, dst2)

        tb.run()

        # get output
        result_data = dst2.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

    def test_002_filter_delay_two_inputs(self):
        # giving the same signal to both the inputs should fetch the same results
        # as above

        tb = self.tb

        sampling_freq = 100
        ntaps = 51
        N = int(ntaps + sampling_freq * 0.10)
        data = sin_source_f(sampling_freq, sampling_freq * 0.10, 1.0, N)
        src1 = blocks.vector_source_f(data)
        dst2 = blocks.vector_sink_c()

        # calculate taps
        taps = filter.firdes.hilbert(ntaps, fft.window.WIN_HAMMING)
        hd = filter.filter_delay_fc(taps)

        expected_result = fir_filter2(data, data, taps, (ntaps - 1) // 2)

        tb.connect(src1, (hd, 0))
        tb.connect(src1, (hd, 1))
        tb.connect(hd, dst2)
        tb.run()

        # get output
        result_data = dst2.data()

        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

    def test_003_filter_delay_two_inputs(self):
        # give two different inputs
        tb = self.tb

        sampling_freq = 100
        ntaps = 51
        N = int(ntaps + sampling_freq * 0.10)

        data1 = sin_source_f(sampling_freq, sampling_freq * 0.10, 1.0, N)
        data2 = cos_source_f(sampling_freq, sampling_freq * 0.10, 1.0, N)
        src1 = blocks.vector_source_f(data1)
        src2 = blocks.vector_source_f(data2)

        taps = filter.firdes.hilbert(ntaps, fft.window.WIN_HAMMING)
        hd = filter.filter_delay_fc(taps)

        expected_result = fir_filter2(data1, data2, taps, (ntaps - 1) // 2)

        dst2 = blocks.vector_sink_c()

        tb.connect(src1, (hd, 0))
        tb.connect(src2, (hd, 1))
        tb.connect(hd, dst2)

        tb.run()

        # get output
        result_data = dst2.data()

        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_filter_delay_fc)
