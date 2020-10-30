#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

import math


def sig_source_f(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [amp * math.cos(2. * math.pi * freq * x) for x in t]
    return y


def sig_source_c(samp_rate, freq, amp, N):
    t = [float(x) / samp_rate for x in range(N)]
    y = [amp * math.cos(2. * math.pi * freq * x) +
         1j * amp * math.sin(2. * math.pi * freq * x) for x in t]
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

        expected_data = amp / math.sqrt(2.0)

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
    gr_unittest.run(test_rms)
