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
from fractions import Fraction
import math
import numpy
import pmt
import threading
import time


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

    def run_tagged(self, op, N, stride, max_noutput_items, gate=None):
        tags = []
        for k in range(0, N, stride):
            t = gr.tag_t()
            t.offset = k
            t.key = pmt.intern('test')
            t.value = pmt.from_uint64(k)
            tags.append(t)
        src = blocks.vector_source_f([0.0] * N, False, 1, tags)
        dst = blocks.vector_sink_f()
        if gate is None:
            self.tb.connect(src, op, dst)
        else:
            self.tb.connect(src, gate, op, dst)
        op.set_max_noutput_items(max_noutput_items)
        return dst

    def check_tags(self, dst, expected):
        # Every tag that maps to produced output must be there, on exactly the
        # offset the default propagation in block_executor would give it.
        nout = len(dst.data())
        expected = [(offset, k) for offset, k in expected if offset < nout]
        self.assertGreater(len(expected), 100)
        result = sorted((t.offset, pmt.to_uint64(t.value)) for t in dst.tags())
        self.assertEqual(expected, result)

    def check_tag_placement(self, rate, max_noutput_items):
        # A small max_noutput_items makes the block produce output for a
        # sample before consuming it, so a tag placed only on consumption
        # can come after its output has gone downstream and get lost.
        N = 20000
        stride = 25
        nfilts = 32
        taps = filter.firdes.low_pass(nfilts, nfilts, 0.4, 0.1)
        op = filter.pfb_arb_resampler_fff(rate, taps, nfilts)
        dst = self.run_tagged(op, N, stride, max_noutput_items)
        self.tb.run()

        r = Fraction(float(numpy.float32(rate)))
        self.check_tags(dst, [(math.floor(k * r + Fraction(1, 2)), k)
                              for k in range(0, N, stride)])

    def test_fff_tags_interp(self):
        self.check_tag_placement(3.57, 96)

    def test_fff_tags_decim(self):
        self.check_tag_placement(0.4, 32)

    def test_fff_tags_set_rate(self):
        # Change the rate while the block is idle in the middle of the
        # stream. Tags not placed yet must be counted from that point with
        # the new rate.
        N = 20000
        N1 = 8000
        stride = 25
        rate1 = 0.7
        rate2 = 2.3
        nfilts = 32
        taps = filter.firdes.low_pass(nfilts, nfilts, 0.4, 0.1)

        class gate(gr.sync_block):
            # passes the first N1 items, then waits for go
            def __init__(self):
                gr.sync_block.__init__(
                    self, 'gate', [numpy.float32], [numpy.float32])
                self.blocked = threading.Event()
                self.go = threading.Event()

            def work(self, input_items, output_items):
                n = len(output_items[0])
                if self.nitems_written(0) < N1:
                    n = min(n, N1 - self.nitems_written(0))
                else:
                    self.blocked.set()
                    self.go.wait()
                output_items[0][:n] = input_items[0][:n]
                return n

        g = gate()
        op = filter.pfb_arb_resampler_fff(rate1, taps, nfilts)
        dst = self.run_tagged(op, N, stride, 64, g)
        self.tb.start()
        self.assertTrue(g.blocked.wait(10))
        # wait until the resampler has used up its input
        counts = None
        stable = 0
        while stable < 5:
            time.sleep(0.05)
            prev, counts = counts, (op.nitems_read(0), op.nitems_written(0))
            stable = stable + 1 if counts == prev else 0
        in0, out0 = counts
        op.set_rate(rate2)
        g.go.set()
        self.tb.wait()

        r1 = Fraction(float(numpy.float32(rate1)))
        r2 = Fraction(float(numpy.float32(rate2)))
        expected = []
        for k in range(0, N, stride):
            offset = math.floor(k * r1 + Fraction(1, 2))
            if k >= N1 or (k >= in0 and offset >= out0):
                offset = out0 + math.floor((k - in0) * r2 + Fraction(1, 2))
            expected.append((offset, k))
        self.check_tags(dst, expected)


if __name__ == '__main__':
    gr_unittest.run(test_pfb_arb_resampler)
