#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy
from gnuradio import gr, gr_unittest, wavelet, analog, blocks
import copy
import math


def sqr(x):
    return x * x


def np2(k):
    m = 0
    n = k - 1
    while n > 0:
        n >>= 1
        m += 1
    return m


class test_classify(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

#     def test_000_(self):
#         src_data = numpy.zeros(10)
#         trg_data = numpy.zeros(10)
#         src = blocks.vector_source_f(src_data)
#         dst = blocks.vector_sink_f()
#         self.tb.connect(src, dst)
#         self.tb.run()
#         rsl_data = dst.data()
#         sum = 0
#         for (u,v) in zip(trg_data, rsl_data):
#             w = u - v
#             sum += w * w
#         sum /= float(len(trg_data))
#         assert sum < 1e-6

    def test_001_(self):
        src_data = numpy.array([-1.0, 1.0, -1.0, 1.0])
        trg_data = src_data * 0.5
        src = blocks.vector_source_f(src_data)
        dst = blocks.vector_sink_f()
        rail = analog.rail_ff(-0.5, 0.5)
        self.tb.connect(src, rail)
        self.tb.connect(rail, dst)
        self.tb.run()
        rsl_data = dst.data()
        sum = 0
        for (u, v) in zip(trg_data, rsl_data):
            w = u - v
            sum += w * w
        sum /= float(len(trg_data))
        assert sum < 1e-6

    def test_002_(self):
        src_data = numpy.array([-1.0,
                                -1.0 / 2.0,
                                -1.0 / 3.0,
                                -1.0 / 4.0,
                                -1.0 / 5.0])
        trg_data = copy.deepcopy(src_data)

        src = blocks.vector_source_f(src_data, False, len(src_data))
        st = blocks.stretch_ff(-1.0 / 5.0, len(src_data))
        dst = blocks.vector_sink_f(len(src_data))
        self.tb.connect(src, st)
        self.tb.connect(st, dst)
        self.tb.run()
        rsl_data = dst.data()
        sum = 0
        for (u, v) in zip(trg_data, rsl_data):
            w = u - v
            sum += w * w
        sum /= float(len(trg_data))
        assert sum < 1e-6

    def test_003_(self):
        src_grid = (0.0, 1.0, 2.0, 3.0, 4.0)
        trg_grid = copy.deepcopy(src_grid)
        src_data = (0.0, 1.0, 0.0, 1.0, 0.0)

        src = blocks.vector_source_f(src_data, False, len(src_grid))
        sq = wavelet.squash_ff(src_grid, trg_grid)
        dst = blocks.vector_sink_f(len(trg_grid))
        self.tb.connect(src, sq)
        self.tb.connect(sq, dst)
        self.tb.run()
        rsl_data = dst.data()
        sum = 0
        for (u, v) in zip(src_data, rsl_data):
            w = u - v
            sum += w * w
        sum /= float(len(src_data))
        assert sum < 1e-6

    def test_004_(self):
        n = 256
        o = 4
        a = numpy.arange(n)
        b = numpy.sin(a * numpy.pi / 16.0).astype(numpy.float32)

        src = blocks.vector_source_f(b, False, n)
        fwd = wavelet.wavelet_ff(n, o, True)
        inv = wavelet.wavelet_ff(n, o, False)
        dst = blocks.vector_sink_f(n)

        self.tb.connect(src, fwd)
        self.tb.connect(fwd, inv)
        self.tb.connect(inv, dst)
        self.tb.run()

        rsl_data = dst.data()
        sum_sq = 0.0
        for (u, v) in zip(b, rsl_data):
            w = u - v
            sum_sq += w * w
        sum_sq /= float(len(b))
        assert sum_sq < 1e-6

    def test_005_(self):

        src_data = (1.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0)

        dwav = numpy.array(src_data)
        wvps = numpy.zeros(3)
        # wavelet power spectrum
        scl = 1.0 / sqr(dwav[0])
        k = 1
        for e in range(len(wvps)):
            wvps[e] = scl * sqr(dwav[k:k + (0o1 << e)]).sum()
            k += 0o1 << e

        src = blocks.vector_source_f(src_data, False, len(src_data))
        kon = wavelet.wvps_ff(len(src_data))
        dst = blocks.vector_sink_f(int(math.ceil(math.log(len(src_data), 2))))

        self.tb.connect(src, kon)
        self.tb.connect(kon, dst)

        self.tb.run()
        snk_data = dst.data()

        sum = 0
        for (u, v) in zip(snk_data, wvps):
            w = u - v
            sum += w * w
        sum /= float(len(snk_data))
        assert sum < 1e-6

    def test_006_(self):
        n = 256
        o = 4
        a = numpy.arange(n)
        b = numpy.sin(a * numpy.pi / 16.0).astype(numpy.float32)

        src = blocks.vector_source_f(b, False, n)
        fwd = wavelet.wavelet_ff(n, o, True)
        dst = blocks.vector_sink_f(n)

        self.tb.connect(src, fwd)
        self.tb.connect(fwd, dst)
        self.tb.run()

        coeffs = numpy.array(dst.data(), dtype=numpy.float32)
        energy_in = numpy.sum(b**2)
        energy_out = numpy.sum(coeffs**2)
        rel_err = abs(energy_in - energy_out) / energy_in
        assert rel_err < 1e-5


if __name__ == '__main__':
    gr_unittest.run(test_classify)
