#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, analog, blocks

class test_fmdet_cf(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_fmdet_cf_001(self):
        # Test set/gets

        fh1 = 10
        fh2 = 20
        fl1 = 1
        fl2 = 2
        scale1 = 3
        scale2 = 4
        op = analog.fmdet_cf(1, fl1, fh1, scale1)

        op.set_freq_range(fl2, fh2)
        lo = op.freq_low()
        hi = op.freq_high()
        f  = op.freq()
        self.assertEqual(fl2, lo)
        self.assertEqual(fh2, hi)
        self.assertEqual(0, f)

        op.set_scale(scale2)
        s = op.scale()
        b = op.bias()
        eb = 0.5*scale2*(hi + lo) / (hi - lo);
        self.assertEqual(scale2, s)
        self.assertAlmostEqual(eb, b)

    # FIXME: This passes QA, but the it's only based off what the
    # block is saying, not what the values should actually be.
    def est_fmdet_cf_002(self):
        N = 100
        src = analog.sig_source_c(1, analog.GR_SIN_WAVE, 0.2, 1)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = analog.fmdet_cf(1, 0.1, 0.3, 0.1)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, head, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()[4:N]
        expected_result = (100-4)*[-0.21755,]
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_fmdet_cf, "test_fmdet_cf.xml")

