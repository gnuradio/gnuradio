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

import time
from gnuradio import gr, gr_unittest, blocks

class test_sample_and_hold(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data        = 10*[0,1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1]
        ctrl_data       = 10*[1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0]
        expected_result = 10*(0,0,0,0,4,5,6,7,8,9,9,9,9,9,9,9,9,9)

        src  = blocks.vector_source_f(src_data)
        ctrl = blocks.vector_source_b(ctrl_data)
        op = blocks.sample_and_hold_ff()
        dst = blocks.vector_sink_f()

        self.tb.connect(src,  (op,0))
        self.tb.connect(ctrl, (op,1))
        self.tb.connect(op, dst)
        self.tb.run()

        result = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result, places=6)

if __name__ == '__main__':
    gr_unittest.run(test_sample_and_hold, "test_sample_and_hold.xml")
