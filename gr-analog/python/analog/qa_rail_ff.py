#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog, blocks


def clip(x, lo, hi):
    if(x < lo):
        return lo
    elif(x > hi):
        return hi
    else:
        return x


class test_rail(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_rail_001(self):
        # Test set/gets

        hi1 = 1
        hi2 = 2
        lo1 = -1
        lo2 = -2

        op = analog.rail_ff(lo1, hi1)

        op.set_hi(hi2)
        h = op.hi()
        self.assertEqual(hi2, h)

        op.set_lo(lo2)
        l = op.lo()
        self.assertEqual(lo2, l)

    def test_rail_002(self):
        lo = -0.75
        hi = 0.90
        src_data = [-2, -1, -0.5, -0.25, 0, 0.25, 0.5, 1, 2]
        expected_result = [clip(x, lo, hi) for x in src_data]

        src = blocks.vector_source_f(src_data)
        op = analog.rail_ff(lo, hi)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 4)


if __name__ == '__main__':
    gr_unittest.run(test_rail)
