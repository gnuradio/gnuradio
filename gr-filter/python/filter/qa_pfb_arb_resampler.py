#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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


def sig_source_f(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [math.sin(2. * math.pi * freq * x) for x in t]
    return y


class test_pfb_arb_resampler(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_fff_000(self):
        N = 500         # number of samples to use
        fs = 5000.0     # baseband sampling rate
        rrate = 2.3421  # resampling rate

        nfilts = 32
        taps = filter.firdes.low_pass_2(
            nfilts,
            nfilts * fs,
            fs / 2,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

        freq = 121.213
        data = sig_source_f(fs, freq, 1, N)
        signal = blocks.vector_source_f(data)
        pfb = filter.pfb_arb_resampler_fff(rrate, taps, nfilts)
        snk = blocks.vector_sink_f()

        self.tb.connect(signal, pfb, snk)
        self.tb.run()

        Ntest = 50
        L = len(snk.data())

        # Get group delay and estimate of phase offset from the filter itself.
        delay = pfb.group_delay()
        phase = pfb.phase_offset(freq, fs)

        # Create a timeline offset by the filter's group delay
        t = [float(x) / (fs * rrate) for x in range(-delay, L - delay)]

        # Data of the sinusoid at frequency freq with the delay and phase
        # offset.
        expected_data = [math.sin(2. * math.pi * freq * x + phase) for x in t]

        dst_data = snk.data()

        self.assertFloatTuplesAlmostEqual(
            expected_data[-Ntest:], dst_data[-Ntest:], 2)

    def test_ccf_000(self):
        N = 5000         # number of samples to use
        fs = 5000.0      # baseband sampling rate
        rrate = 2.4321   # resampling rate

        nfilts = 32
        taps = filter.firdes.low_pass_2(
            nfilts,
            nfilts * fs,
            fs / 2,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

        freq = 211.123
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        pfb = filter.pfb_arb_resampler_ccf(rrate, taps, nfilts)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, pfb, snk)
        self.tb.run()

        Ntest = 50
        L = len(snk.data())

        # Get group delay and estimate of phase offset from the filter itself.
        delay = pfb.group_delay()
        phase = pfb.phase_offset(freq, fs)

        # Create a timeline offset by the filter's group delay
        t = [float(x) / (fs * rrate) for x in range(-delay, L - delay)]

        # Data of the sinusoid at frequency freq with the delay and phase
        # offset.
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
            expected_data[-Ntest:], dst_data[-Ntest:], 2)

    def test_ccf_001(self):
        N = 50000        # number of samples to use
        fs = 5000.0      # baseband sampling rate
        rrate = 0.75     # resampling rate

        nfilts = 32
        taps = filter.firdes.low_pass_2(
            nfilts,
            nfilts * fs,
            fs / 4,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

        freq = 211.123
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        pfb = filter.pfb_arb_resampler_ccf(rrate, taps, nfilts)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, pfb, snk)
        self.tb.run()

        Ntest = 50
        L = len(snk.data())

        # Get group delay and estimate of phase offset from the filter itself.
        delay = pfb.group_delay()
        phase = pfb.phase_offset(freq, fs)

        # Create a timeline offset by the filter's group delay
        t = [float(x) / (fs * rrate) for x in range(-delay, L - delay)]

        # Data of the sinusoid at frequency freq with the delay and phase
        # offset.
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
            expected_data[-Ntest:], dst_data[-Ntest:], 2)

    def test_ccc_000(self):
        N = 5000         # number of samples to use
        fs = 5000.0      # baseband sampling rate
        rrate = 3.4321   # resampling rate

        nfilts = 32
        taps = filter.firdes.complex_band_pass_2(
            nfilts,
            nfilts * fs,
            50,
            400,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

        freq = 211.123
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        pfb = filter.pfb_arb_resampler_ccc(rrate, taps, nfilts)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, pfb, snk)
        self.tb.run()

        Ntest = 50
        L = len(snk.data())

        # Get group delay and estimate of phase offset from the filter itself.
        delay = pfb.group_delay()
        phase = pfb.phase_offset(freq, fs)

        # Create a timeline offset by the filter's group delay
        t = [float(x) / (fs * rrate) for x in range(-delay, L - delay)]

        # Data of the sinusoid at frequency freq with the delay and phase
        # offset.
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
            expected_data[-Ntest:], dst_data[-Ntest:], 2)

    def test_ccc_001(self):
        N = 50000        # number of samples to use
        fs = 5000.0      # baseband sampling rate
        rrate = 0.715    # resampling rate

        nfilts = 32
        taps = filter.firdes.complex_band_pass_2(
            nfilts,
            nfilts * fs,
            50,
            400,
            fs / 10,
            attenuation_dB=80,
            window=fft.window.WIN_BLACKMAN_hARRIS)

        freq = 211.123
        data = sig_source_c(fs, freq, 1, N)
        signal = blocks.vector_source_c(data)
        pfb = filter.pfb_arb_resampler_ccc(rrate, taps, nfilts)
        snk = blocks.vector_sink_c()

        self.tb.connect(signal, pfb, snk)
        self.tb.run()

        Ntest = 50
        L = len(snk.data())

        # Get group delay and estimate of phase offset from the filter itself.
        delay = pfb.group_delay()
        phase = pfb.phase_offset(freq, fs)

        # Create a timeline offset by the filter's group delay
        t = [float(x) / (fs * rrate) for x in range(-delay, L - delay)]

        # Data of the sinusoid at frequency freq with the delay and phase
        # offset.
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
            expected_data[-Ntest:], dst_data[-Ntest:], 2)


if __name__ == '__main__':
    gr_unittest.run(test_pfb_arb_resampler)
