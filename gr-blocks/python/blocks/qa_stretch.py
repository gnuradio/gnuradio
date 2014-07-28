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

class test_stretch(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_stretch_01(self):
        tb = self.tb

        data = 10*[1,]
        data0 = map(lambda x: x/20.0, data)
        data1 = map(lambda x: x/10.0, data)

        expected_result0 = 10*[0.05,]
        expected_result1 = 10*[0.1,]

        src0 = blocks.vector_source_f(data0, False)
        src1 = blocks.vector_source_f(data1, False)
        inter = blocks.streams_to_vector(gr.sizeof_float, 2)
        op = blocks.stretch_ff(0.1, 2)
        deinter = blocks.vector_to_streams(gr.sizeof_float, 2)
        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()

        tb.connect(src0, (inter,0))
        tb.connect(src1, (inter,1))
        tb.connect(inter, op)
        tb.connect(op, deinter)
        tb.connect((deinter,0), dst0)
        tb.connect((deinter,1), dst1)
        tb.run()

        dst0_data = dst0.data()
        dst1_data = dst1.data()

        self.assertFloatTuplesAlmostEqual(expected_result0, dst0_data, 4)
        self.assertFloatTuplesAlmostEqual(expected_result1, dst1_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_stretch, "test_stretch.xml")
