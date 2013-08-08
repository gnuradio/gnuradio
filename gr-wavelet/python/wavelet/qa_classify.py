#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import numpy
from gnuradio import gr, gr_unittest, wavelet, analog, blocks
import copy
#import pygsl.wavelet as wavelet # FIXME: pygsl not checked for in config
import math

def sqr(x):
    return x*x

def np2(k):
    m = 0
    n = k - 1
    while n > 0:
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
                                -1.0/2.0,
                                -1.0/3.0,
                                -1.0/4.0,
                                -1.0/5.0])
        trg_data = copy.deepcopy(src_data)

        src = blocks.vector_source_f(src_data, False, len(src_data))
        st = blocks.stretch_ff(-1.0/5.0, len(src_data))
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

#    def test_004_(self): # FIXME: requires pygsl
#
#        n = 256
#        o = 4
#        ws = wavelet.workspace(n)
#        w = wavelet.daubechies(o)
#
#        a = numpy.arange(n)
#        b = numpy.sin(a*numpy.pi/16.0)
#        c = w.transform_forward(b, ws)
#        d = w.transform_inverse(c, ws)
#
#        src = gr.vector_source_f(b, False, n)
#        wv = wavelet.wavelet_ff(n, o, True)
#        src = blocks.vector_source_f(b, False, n)
#        wv = wavelet.wavelet_ff(n, o, True)
#
#        dst = blocks.vector_sink_f(n)
#        self.tb.connect(src, wv)
#        self.tb.connect(wv, dst)
#        self.tb.run()
#        e = dst.data()
#
#        sum = 0
#        for (u, v) in zip(c, e):
#            w = u - v
#            sum += w * w
#        sum /= float(len(c))
#        assert sum < 1e-6

    def test_005_(self):

        src_data = (1.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0)

        dwav = numpy.array(src_data)
        wvps = numpy.zeros(3)
        # wavelet power spectrum
        scl = 1.0/sqr(dwav[0])
        k = 1
        for e in range(len(wvps)):
            wvps[e] = scl*sqr(dwav[k:k+(01<<e)]).sum()
            k += 01<<e

        src = blocks.vector_source_f(src_data, False, len(src_data))
        kon = wavelet.wvps_ff(len(src_data))
        dst = blocks.vector_sink_f(int(math.ceil(math.log(len(src_data), 2))))

        self.tb.connect(src, kon)
        self.tb.connect(kon, dst)

        self.tb.run()
        snk_data = dst.data()

        sum = 0
        for (u,v) in zip(snk_data, wvps):
            w = u - v
            sum += w * w
        sum /= float(len(snk_data))
        assert sum < 1e-6

if __name__ == '__main__':
   gr_unittest.run(test_classify, "test_classify.xml")
