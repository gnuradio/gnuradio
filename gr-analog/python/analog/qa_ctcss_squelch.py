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

class test_ctcss_squelch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_ctcss_squelch_001(self):
        # Test set/gets

        rate = 1
        rate2 = 2
        freq = 100
        level = 0.5
        length = 1
        ramp = 1
        ramp2 = 2
        gate = True
        gate2 = False

        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)

        op.set_ramp(ramp2)
        r = op.ramp()
        self.assertEqual(ramp2, r)

        op.set_gate(gate2)
        g = op.gate()
        self.assertEqual(gate2, g)

    def test_ctcss_squelch_002(self):
        # Test runtime, gate=True
        rate = 1
        freq = 100
        level = 0.0
        length = 1
        ramp = 1
        gate = True

        src_data = map(lambda x: float(x)/10.0, range(1, 40))
        expected_result = src_data
        expected_result[0] = 0

        src = blocks.vector_source_f(src_data)
        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)

    def test_ctcss_squelch_003(self):
        # Test runtime, gate=False
        rate = 1
        freq = 100
        level = 0.5
        length = 1
        ramp = 1
        gate = False

        src_data = map(lambda x: float(x)/10.0, range(1, 40))
        src = blocks.vector_source_f(src_data)
        op = analog.ctcss_squelch_ff(rate, freq, level,
                                     length, ramp, gate)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = src_data
        expected_result[0:5] = [0, 0, 0, 0, 0]

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_ctcss_squelch, "test_ctcss_squelch.xml")

