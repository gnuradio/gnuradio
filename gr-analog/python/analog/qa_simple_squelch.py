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

class test_simple_squelch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_simple_squelch_001(self):
        # Test set/gets

        alpha = 0.0001

        thr1 = 10
        thr2 = 20

        op = analog.simple_squelch_cc(thr1, alpha)

        op.set_threshold(thr2)
        t = op.threshold()
        self.assertEqual(thr2, t)

    def test_simple_squelch_002(self):
        alpha = 0.0001
        thr = -25

        src_data = map(lambda x: float(x)/10.0, range(1, 40))
        src = blocks.vector_source_c(src_data)
        op = analog.simple_squelch_cc(thr, alpha)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        expected_result = src_data
        expected_result[0:20] = 20*[0,]

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_simple_squelch, "test_simple_squelch.xml")

