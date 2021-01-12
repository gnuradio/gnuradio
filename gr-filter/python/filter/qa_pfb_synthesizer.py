#!/usr/bin/env python
#
# Copyright 2012-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, fft, filter, blocks

import math


def sig_source_c(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.cos(2. * math.pi * freq * x) +
         1j * math.sin(2. * math.pi * freq * x) for x in t]
    return y


class test_pfb_synthesizer(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 10000         # number of samples to use
        M = 5            # Number of channels
        fs = 1000        # baseband sampling rate
        ofs = M * fs       # input samp rate to decimator

        taps = filter.firdes.low_pass_2(
            M,
            ofs,
            fs / 2,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

        signals = list()
        freqs = [0, 100, 200, -200, -100]
        for i in range(len(freqs)):
            data = sig_source_c(fs, freqs[i], 1, N)
            signals.append(blocks.vector_source_c(data))

        pfb = filter.pfb_synthesizer_ccf(M, taps)
        snk = blocks.vector_sink_c()

        for i in range(M):
            self.tb.connect(signals[i], (pfb, i))

        self.tb.connect(pfb, snk)

        self.tb.run()

        # Adjusted phase rotations for data
        p0 = 0
        p1 = 2 * math.pi * 1.0 / M
        p2 = 2 * math.pi * 2.0 / M
        p3 = 2 * math.pi * 3.0 / M
        p4 = 2 * math.pi * 4.0 / M

        Ntest = 1000
        L = len(snk.data())
        t = [float(x) / ofs for x in range(L)]

        # Create known data as sum of complex sinusoids at freqs
        # of the output channels.
        freqs = [-2200, -1100, 0, 1100, 2200]
        expected_data = len(t) * [0, ]
        for i in range(len(t)):
            expected_data[i] = math.cos(2. * math.pi * freqs[0] * t[i] + p3) + \
                1j * math.sin(2. * math.pi * freqs[0] * t[i] + p3) + \
                math.cos(2. * math.pi * freqs[1] * t[i] + p4) + \
                1j * math.sin(2. * math.pi * freqs[1] * t[i] + p4) + \
                math.cos(2. * math.pi * freqs[2] * t[i] + p0) + \
                1j * math.sin(2. * math.pi * freqs[2] * t[i] + p0) + \
                math.cos(2. * math.pi * freqs[3] * t[i] + p1) + \
                1j * math.sin(2. * math.pi * freqs[3] * t[i] + p1) + \
                math.cos(2. * math.pi * freqs[4] * t[i] + p2) + \
                1j * math.sin(2. * math.pi * freqs[4] * t[i] + p2)
        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(expected_data[2000:2000 + Ntest],
                                            dst_data[2000:2000 + Ntest], 4)


if __name__ == '__main__':
    gr_unittest.run(test_pfb_synthesizer)
