#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, filter, blocks
import math

def sig_source_f(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: math.sin(2.*math.pi*freq*x), t)
    return y

def fir_filter(x, taps):
    y = []
    x2 = (len(taps)-1)*[0,] + x
    delay = (len(taps)-1)/2
    for i in range(len(x)):
        yi = 0
        for j in range(len(taps)):
            yi += taps[len(taps)-1-j] * x2[i+j]
        y.append(complex(x2[i+delay], yi))
    return y

class test_hilbert(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_hilbert(self):
        tb = self.tb
        ntaps = 51
        sampling_freq = 100

        N = int(ntaps + sampling_freq * 0.10)
        data = sig_source_f(sampling_freq, sampling_freq * 0.10, 1.0, N)
        src1 = blocks.vector_source_f(data)

        taps = filter.firdes.hilbert(ntaps, filter.firdes.WIN_HAMMING)
        expected_result = fir_filter(data, taps)

        hilb = filter.hilbert_fc(ntaps)
        dst1 = blocks.vector_sink_c()
        tb.connect(src1, hilb)
        tb.connect(hilb, dst1)
        tb.run()
        dst_data = dst1.data()

        self.assertComplexTuplesAlmostEqual(expected_result, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_hilbert, "test_hilbert.xml")
