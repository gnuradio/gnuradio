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


class test_pfb_interpolator(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        M = 5            # Number of channels
        fs = 1000        # baseband sampling rate
        ofs = M * fs       # output samp rate of interpolator

        taps = filter.firdes.low_pass_2(
            M,
            ofs,
            fs / 4,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

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
        t = [float(x) / ofs for x in range(0, L)]

        # Create known data as complex sinusoids for the baseband freq
        # of the extracted channel is due to decimator output order.
        expected_data = [
            math.cos(
                2. *
                math.pi *
                freq *
                x +
                phase) +
            1j *
            math.sin(
                2. *
                math.pi *
                freq *
                x +
                phase) for x in t]

        dst_data = snk.data()

        self.assertComplexTuplesAlmostEqual(
            expected_data[-Ntest:], dst_data[-Ntest:], 4)


if __name__ == '__main__':
    gr_unittest.run(test_pfb_interpolator)
