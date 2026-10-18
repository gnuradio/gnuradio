#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog, blocks, channels
import numpy


def bessel_j0(x):
    # J0(x) = 1/pi * integral_0^pi cos(x sin t) dt, evaluated by midpoint rule
    t = (numpy.arange(2000) + 0.5) * numpy.pi / 2000
    return numpy.cos(numpy.outer(x, numpy.sin(t))).mean(axis=1)


def xcorr(a, b, lags):
    # time-averaged E[a(t) b(t + lag)] for non-negative lags
    return numpy.array([numpy.mean(a[:len(a) - L] * b[L:]) for L in lags])


class test_fading_model(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def run_channel(self, op, nsamps):
        # constant input, so the output is the channel gain
        src = analog.sig_source_c(1, analog.GR_CONST_WAVE, 0, 0, 1)
        head = blocks.head(gr.sizeof_gr_complex, nsamps)
        snk = blocks.vector_sink_c()
        self.tb.connect(src, head, op, snk)
        self.tb.run()
        self.tb = gr.top_block()
        return numpy.array(snk.data())

    def test_000(self):
        N = 1000         # number of samples to use
        fs = 1000        # baseband sampling rate
        freq = 100

        fDTs = 0.01
        K = 4
        signal = analog.sig_source_c(fs, analog.GR_SIN_WAVE, freq, 1)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = channels.fading_model(8, fDTs=fDTs, LOS=True,
                                   K=K, seed=0)
        snk = blocks.vector_sink_c()
        snk1 = blocks.vector_sink_c()

        self.assertAlmostEqual(K, op.K(), 4)
        self.assertAlmostEqual(fDTs, op.fDTs(), 4)

        #self.tb.connect(signal, head, op, snk)
        #self.tb.connect(op, snk1)
        # self.tb.run()

        #dst_data = snk.data()
        #exp_data = snk1.data()
        #self.assertComplexTuplesAlmostEqual(exp_data, dst_data, 5)

    def test_001_rayleigh_statistics(self):
        # Alimohammad et al., Model III: unit power, J0 autocorrelation and
        # uncorrelated I/Q within a single realization
        fDTs = 0.01
        lags = numpy.arange(0, 800, 10)
        ref = bessel_j0(2 * numpy.pi * fDTs * lags)
        for seed in range(3):
            h = self.run_channel(channels.fading_model(
                8, fDTs=fDTs, LOS=False, K=0, seed=seed), 2000000)
            i, q = h.real, h.imag
            self.assertAlmostEqual(numpy.mean(numpy.abs(h)**2), 1.0, delta=0.1)
            for acf in (xcorr(i, i, lags), xcorr(q, q, lags)):
                rms_err = numpy.sqrt(numpy.mean((acf / 0.5 - ref)**2))
                self.assertLess(rms_err, 0.08)
            ccf = numpy.concatenate((xcorr(i, q, lags), xcorr(q, i, lags)))
            self.assertLess(numpy.max(numpy.abs(ccf)) / 0.5, 0.1)

    def test_001b_doppler_spectrum(self):
        # Doppler spectrum is band-limited to |f| <= fD and symmetric
        fDTs = 0.01
        for seed in range(3):
            h = self.run_channel(channels.fading_model(
                8, fDTs=fDTs, LOS=False, K=0, seed=seed), 2**21)
            psd = numpy.abs(numpy.fft.fft(h))**2
            f = numpy.fft.fftfreq(len(h))
            total = numpy.sum(psd)
            self.assertLess(numpy.sum(psd[numpy.abs(f) > 1.1 * fDTs]) / total, 1e-3)
            pos = numpy.sum(psd[f > 0]) / total
            self.assertAlmostEqual(pos, 0.5, delta=0.05)

    def test_002_rician_statistics(self):
        # unit power; E|h|^4 / E^2|h|^2 = (2 + 4K + K^2) / (K + 1)^2
        K = 4.0
        for seed in range(3):
            h = self.run_channel(channels.fading_model(
                8, fDTs=0.01, LOS=True, K=K, seed=seed), 2000000)
            p = numpy.abs(h)**2
            self.assertAlmostEqual(numpy.mean(p), 1.0, delta=0.1)
            self.assertAlmostEqual(numpy.mean(p**2) / numpy.mean(p)**2,
                                   (2 + 4 * K + K**2) / (K + 1)**2, delta=0.03)

    def check_fractional_delay_gain(self, make_selective):
        # single path equals the flat fader output (same seed) times the DC
        # gain of the truncated sinc interpolator
        ntaps = 8
        nsamps = 10000
        h = self.run_channel(channels.fading_model(
            8, fDTs=0.01, LOS=False, K=0, seed=5), nsamps)
        for delay in (2.0, 2.0001, 1.9999999, 2.25, 2.5, 2.75, 3.0):
            y = self.run_channel(make_selective(delay, ntaps), nsamps)
            gain = y[ntaps:] / h[ntaps:]
            expected = numpy.sum(numpy.sinc(numpy.arange(ntaps) - delay))
            self.assertLess(numpy.std(gain), 1e-3)
            self.assertAlmostEqual(abs(numpy.mean(gain)), expected, delta=1e-3)

    def test_003_selective_fractional_delay(self):
        self.check_fractional_delay_gain(
            lambda delay, ntaps: channels.selective_fading_model(
                8, 0.01, False, 0, 5, [delay], [1.0], ntaps))

    def test_004_selective2_fractional_delay(self):
        self.check_fractional_delay_gain(
            lambda delay, ntaps: channels.selective_fading_model2(
                8, 0.01, False, 0, 5, [delay], [0.0], [0.0], [1.0], ntaps))

    def test_005_selective2_seed(self):
        # with one path, y / h is the interpolator gain, which depends only on
        # the delay random walk
        def gain(seed):
            h = self.run_channel(channels.fading_model(
                8, 0.01, False, 0, seed), 20000)
            y = self.run_channel(channels.selective_fading_model2(
                8, 0.01, False, 0, seed, [3.0], [0.01], [0.5], [1.0], 8), 20000)
            return y[8:] / h[8:]
        a = gain(1)
        self.assertComplexTuplesAlmostEqual(a, gain(1), 5)
        self.assertGreater(numpy.max(numpy.abs(a - gain(2))), 0.01)

    def run_path_gains(self, op, npaths, spacing, nsamps):
        # impulse train input; path j gain is read at offset 4 * j
        x = numpy.zeros(nsamps, dtype=complex)
        x[::spacing] = 1
        src = blocks.vector_source_c(x.tolist())
        snk = blocks.vector_sink_c()
        self.tb.connect(src, op, snk)
        self.tb.run()
        self.tb = gr.top_block()
        y = numpy.array(snk.data())
        return [y[spacing + 4 * j::spacing] for j in range(npaths)]

    def test_007_selective_paths(self):
        # LOS on the first path only; paths uncorrelated
        delays = [0.0, 4.0, 8.0]
        op = channels.selective_fading_model(
            8, 0.01, True, 100.0, 11, delays, [1.0, 1.0, 1.0], 12)
        g = self.run_path_gains(op, 3, 16, 2**20)
        p = [numpy.abs(gj)**2 for gj in g]
        # E|h|^4 / E^2|h|^2: 2 for Rayleigh, ~1 for K = 100
        self.assertLess(numpy.mean(p[0]**2) / numpy.mean(p[0])**2, 1.05)
        for j in (1, 2):
            self.assertAlmostEqual(numpy.mean(p[j]**2) / numpy.mean(p[j])**2,
                                   2.0, delta=0.3)
        for a, b in ((0, 1), (0, 2), (1, 2)):
            rho = numpy.abs(numpy.mean(g[a] * numpy.conj(g[b]))) / \
                numpy.sqrt(numpy.mean(p[a]) * numpy.mean(p[b]))
            self.assertLess(rho, 0.1)

    def test_006_set_fDTs(self):
        op = channels.fading_model(8, fDTs=0.01, LOS=False, K=0, seed=0)
        op.set_fDTs(0.001)
        self.assertAlmostEqual(op.fDTs(), 0.001, 6)
        self.assertAlmostEqual(op.step(), 1e-6, 12)


if __name__ == '__main__':
    gr_unittest.run(test_fading_model)
