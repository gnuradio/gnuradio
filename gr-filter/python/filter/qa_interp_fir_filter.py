#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, filter, blocks

import math


class test_interp_fir_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_fff(self):
        taps = [1, 10, 100, 1000, 10000]
        src_data = (0, 2, 3, 5, 7, 11, 13, 17)
        interpolation = 3
        xr = (0, 0, 0,
              2, 20, 200, 2003, 20030,
              300, 3005, 30050,
              500, 5007, 50070,
              700, 7011, 70110,
              1100, 11013, 110130,
              1300, 13017, 130170)
        expected_result = [float(x) for x in xr]

        src = blocks.vector_source_f(src_data)
        op = filter.interp_fir_filter_fff(interpolation, taps)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        L = min(len(result_data), len(expected_result))
        self.assertEqual(expected_result[0:L], result_data[0:L])


if __name__ == '__main__':
    gr_unittest.run(test_interp_fir_filter)
