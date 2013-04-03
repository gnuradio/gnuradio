#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, blocks

import math

def sig_source_f(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: amp*math.cos(2.*math.pi*freq*x), t)
    return y

def sig_source_c(samp_rate, freq, amp, N):
    t = map(lambda x: float(x)/samp_rate, xrange(N))
    y = map(lambda x: amp*math.cos(2.*math.pi*freq*x) + \
                1j*amp*math.sin(2.*math.pi*freq*x), t)
    return y

class test_rms(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_ff(self):
        amp = 2
        src_data = sig_source_f(1, 0.01, amp, 200)
        N = 750000

        expected_data = amp/math.sqrt(2.0)

        src = blocks.vector_source_f(src_data, True)
        head = blocks.head(gr.sizeof_float, N)
        op = blocks.rms_ff(0.0001)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, head, op, dst)
        self.tb.run()

        dst_data = dst.data()
        self.assertAlmostEqual(dst_data[-1], expected_data, 4)

    def test_cf(self):
        amp = 4
        src_data = sig_source_c(1, 0.01, amp, 200)
        N = 750000

        expected_data = amp

        src = blocks.vector_source_c(src_data, True)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = blocks.rms_cf(0.0001)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, head, op, dst)
        self.tb.run()

        dst_data = dst.data()
        self.assertAlmostEqual(dst_data[-1], expected_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_rms, "test_rms.xml")
