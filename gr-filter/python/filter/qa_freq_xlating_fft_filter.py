#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr, gr_unittest, filter, blocks

import cmath, math

def fir_filter(x, taps, decim=1):
    y = []
    x2 = (len(taps)-1)*[0,] + x
    for i in range(0, len(x), decim):
        yi = 0
        for j in range(len(taps)):
            yi += taps[len(taps)-1-j] * x2[i+j]
        y.append(yi)
    return y

def sig_source_s(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: int(100*math.sin(2.*math.pi*freq*x)), t)
    return y

def sig_source_c(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.cos(2.*math.pi*freq*x) + \
                1j*math.sin(2.*math.pi*freq*x), t)
    return y

def mix(lo, data):
    y = [lo_i*data_i for lo_i, data_i in zip(lo, data)]
    return y

class test_freq_xlating_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def generate_ccf_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.low_pass(1, fs, bw, bw/4)
        times = xrange(1024)
        self.src_data = map(lambda t: cmath.exp(-2j*cmath.pi*fc/fs*(t/100.0)), times)

    def generate_ccc_source(self):
        self.fs = fs = 1
        self.fc = fc = 0.3
        self.bw = bw = 0.1
        self.taps = filter.firdes.complex_band_pass(1, fs, -bw/2, bw/2, bw/4)
        times = xrange(1024)
        self.src_data = map(lambda t: cmath.exp(-2j*cmath.pi*fc/fs*(t/100.0)), times)

    def assert_fft_ok(self, expected_result, result_data):
        expected_result = expected_result[:len(result_data)]
        self.assertComplexTuplesAlmostEqual2 (expected_result, result_data,
                                              abs_eps=1e-9, rel_eps=1.5e-3)


    def test_fft_filter_ccf_001(self):
        self.generate_ccf_source()

        decim = 1
        lo = sig_source_c(self.fs, -self.fc, 1, len(self.src_data))
        despun = mix(lo, self.src_data)
        expected_data = fir_filter(despun, self.taps, decim)

        src = blocks.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fft_filter_ccc(decim, self.taps, self.fc, self.fs)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assert_fft_ok(expected_data, result_data)

    def test_fft_filter_ccf_002(self):
        self.generate_ccf_source()

        decim = 4
        lo = sig_source_c(self.fs, -self.fc, 1, len(self.src_data))
        despun = mix(lo, self.src_data)
        expected_data = fir_filter(despun, self.taps, decim)

        src = blocks.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fft_filter_ccc(decim, self.taps, self.fc, self.fs)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assert_fft_ok(expected_data, result_data)

    def test_fft_filter_ccc_001(self):
        self.generate_ccc_source()

        decim = 1
        lo = sig_source_c(self.fs, -self.fc, 1, len(self.src_data))
        despun = mix(lo, self.src_data)
        expected_data = fir_filter(despun, self.taps, decim)

        src = blocks.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fft_filter_ccc(decim, self.taps, self.fc, self.fs)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assert_fft_ok(expected_data, result_data)

    def test_fft_filter_ccc_002(self):
        self.generate_ccc_source()

        decim = 4
        lo = sig_source_c(self.fs, -self.fc, 1, len(self.src_data))
        despun = mix(lo, self.src_data)
        expected_data = fir_filter(despun, self.taps, decim)

        src = blocks.vector_source_c(self.src_data)
        op  = filter.freq_xlating_fft_filter_ccc(decim, self.taps, self.fc, self.fs)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assert_fft_ok(expected_data, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_freq_xlating_filter, "test_freq_xlating_filter.xml")
