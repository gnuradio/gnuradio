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

class test_transcendental(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_01(self):
        tb = self.tb

        data = 100*[0,]
        expected_result = 100*[1,]

        src = blocks.vector_source_f(data, False)
        op = blocks.transcendental("cos", "float")
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_02(self):
        tb = self.tb

        data = 100*[3,]
        expected_result = 100*[math.log10(3),]

        src = blocks.vector_source_f(data, False)
        op = blocks.transcendental("log10", "float")
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

    def test_03(self):
        tb = self.tb

        data = 100*[3,]
        expected_result = 100*[math.tanh(3),]

        src = blocks.vector_source_f(data, False)
        op = blocks.transcendental("tanh", "float")
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()

        dst_data = dst.data()

        self.assertFloatTuplesAlmostEqual(expected_result, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_transcendental, "test_transcendental.xml")
