#!/usr/bin/env python3
#
# Copyright 2012-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, fft, filter, blocks, streamops, math as grmath
from gnuradio.kernel.filter import firdes
from gnuradio.kernel.fft import window
import math
import cmath


def sig_source_c(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.cos(2. * math.pi * freq * x) +
         1j * math.sin(2. * math.pi * freq * x) for x in t]
    return y


class test_pfb_channelizer(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()
        self.freqs = [110., -513., 203., -230, 121]
        # Number of channels to channelize.
        self.M = len(self.freqs)
        # Number of samples to use.
        self.N = 1000
        # Baseband sampling rate.
        self.fs = 5000
        # Input samp rate to channelizer.
        self.ifs = self.M * self.fs

        self.taps = firdes.low_pass_2(
            1, self.ifs, self.fs / 2, self.fs / 10,
            attenuation_dB=80,
            window=window.WIN_BLACKMAN_hARRIS)

        self.Ntest = 50

    def tearDown(self):
        self.tb = None

    def test_0000(self):
        self.check_channelizer(filter.pfb_channelizer_cc(
            self.M, taps=self.taps, oversample_rate=1))


    # def test_0001(self):
    #     self.check_channelizer(filter.pfb.channelizer_hier_ccf(
    #         self.M, n_filterbanks=1, taps=self.taps))

    # def test_0002(self):
    #     """Test roundig error handling for oversample rate (ok)."""
    #     channels, oversample = 36, 25.
    #     filter.pfb.channelizer_ccf(channels, taps=self.taps,
    #                                oversample_rate=channels / oversample)

    def test_0003(self):
        """Test roundig error handling for oversample rate, (bad)."""
        # pybind11 raises ValueError instead of TypeError
        self.assertRaises(ValueError,
                          filter.pfb_channelizer_cc,
                          36, taps=self.taps, oversample_rate=10.1334)


    def check_channelizer(self, channelizer_block):
        signals = list()
        add = grmath.add_cc(len(self.freqs))
        for i in range(len(self.freqs)):
            f = self.freqs[i] + i * self.fs
            data = sig_source_c(self.ifs, f, 1, self.N)
            signals.append(blocks.vector_source_c(data))
            self.tb.connect(signals[i],0 , add, i)

        s2ss = streamops.deinterleave(self.M)

        # self.tb.connect(add, 0, s2ss, 0)
        self.tb.connect(add, channelizer_block)

        snks = list()
        for i in range(self.M):
            snks.append(blocks.vector_sink_c())
            # self.tb.connect(s2ss,i, channelizer_block,i)
            self.tb.connect(channelizer_block, i, snks[i],0)

        self.tb.run()

        L = len(snks[0].data())

        expected_data = self.get_expected_data(L)
        received_data = [snk.data() for snk in snks]

        for expected, received in zip(expected_data, received_data):
            self.compare_data(expected, received)

    def compare_data(self, expected, received):
        Ntest = 50
        expected = expected[-Ntest:]
        received = received[-Ntest:]
        expected = [x / expected[0] for x in expected]
        received = [x / received[0] for x in received]
        self.assertComplexTuplesAlmostEqual(expected, received, 3)

    def get_freq(self, data):
        freqs = []
        for r1, r2 in zip(data[:-1], data[1:]):
            diff = cmath.phase(r1) - cmath.phase(r2)
            if diff > math.pi:
                diff -= 2 * math.pi
            if diff < -math.pi:
                diff += 2 * math.pi
            freqs.append(diff)
        freq = float(sum(freqs)) / len(freqs)
        freq /= 2 * math.pi
        return freq

    def get_expected_data(self, L):

        # Filter delay is the normal delay of each arm
        tpf = math.ceil(len(self.taps) / float(self.M))
        delay = -(tpf - 1.0) / 2.0
        delay = int(delay)

        # Create a time scale that's delayed to match the filter delay
        t = [float(x) / self.fs for x in range(delay, L + delay)]

        # Create known data as complex sinusoids at the different baseband freqs
        # the different channel numbering is due to channelizer output order.
        expected_data = [[math.cos(2. *
                                   math.pi *
                                   f *
                                   x) +
                          1j *
                          math.sin(2. *
                                   math.pi *
                                   f *
                                   x) for x in t] for f in self.freqs]
        return expected_data


if __name__ == '__main__':
    gr_unittest.run(test_pfb_channelizer)
