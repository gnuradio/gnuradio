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

class test_dpll_bb(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_dpll_bb_001(self):
        # Test set/gets

        period = 1.0
        gain = 0.1
        op = analog.dpll_bb(period, gain)

        op.set_gain(0.2)
        g = op.gain()
        self.assertAlmostEqual(g, 0.2)

        f = op.freq()
        self.assertEqual(1/period, f)

        d0 = 1.0 - 0.5*f;
        d1 = op.decision_threshold()
        self.assertAlmostEqual(d0, d1)

        p = op.phase()
        self.assertEqual(0, p)

    def test_dpll_bb_002(self):
        period = 4
        gain = 0.1

        src_data = 10*((period-1)*[0,] + [1,])
        expected_result = src_data

        src = blocks.vector_source_b(src_data)
        op = analog.dpll_bb(period, gain)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_dpll_bb, "test_dpll_bb.xml")

