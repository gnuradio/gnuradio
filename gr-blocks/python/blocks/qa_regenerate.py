#!/usr/bin/env python
#
# Copyright 2007,2010,2013 Free Software Foundation, Inc.
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

class test_regenerate(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_regen1(self):
        tb = self.tb

        data = [0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        expected_result = (0, 0, 0,
                           1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                           1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)


        src = blocks.vector_source_b(data, False)
        regen = blocks.regenerate_bb(5, 2)
        dst = blocks.vector_sink_b()

        tb.connect(src, regen)
        tb.connect(regen, dst)
        tb.run()

        dst_data = dst.data()

        self.assertEqual(expected_result, dst_data)

    def test_regen2(self):
        tb = self.tb

        data = 200*[0,]
        data[9] = 1
        data[99] = 1

        expected_result = 200*[0,]
        expected_result[9]   = 1
        expected_result[19]  = 1
        expected_result[29]  = 1
        expected_result[39]  = 1

        expected_result[99]  = 1
        expected_result[109]  = 1
        expected_result[119]  = 1
        expected_result[129]  = 1

        src = blocks.vector_source_b(data, False)
        regen = blocks.regenerate_bb(10, 3)
        dst = blocks.vector_sink_b()

        tb.connect(src, regen)
        tb.connect(regen, dst)
        tb.run ()

        dst_data = dst.data()

        self.assertEqual(tuple(expected_result), dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_regenerate, "test_regenerate.xml")
