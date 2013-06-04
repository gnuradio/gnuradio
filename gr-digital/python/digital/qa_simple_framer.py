#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, blocks

class test_simple_framer(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_simple_framer_001(self):
        src_data = (0x00, 0x11, 0x22, 0x33,
                    0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb,
                    0xcc, 0xdd, 0xee, 0xff)

        expected_result = (
            0xac, 0xdd, 0xa4, 0xe2, 0xf2, 0x8c, 0x20, 0xfc, 0x00, 0x00, 0x11, 0x22, 0x33, 0x55,
            0xac, 0xdd, 0xa4, 0xe2, 0xf2, 0x8c, 0x20, 0xfc, 0x01, 0x44, 0x55, 0x66, 0x77, 0x55,
            0xac, 0xdd, 0xa4, 0xe2, 0xf2, 0x8c, 0x20, 0xfc, 0x02, 0x88, 0x99, 0xaa, 0xbb, 0x55,
            0xac, 0xdd, 0xa4, 0xe2, 0xf2, 0x8c, 0x20, 0xfc, 0x03, 0xcc, 0xdd, 0xee, 0xff, 0x55)

        src = blocks.vector_source_b(src_data)
        op = digital.simple_framer(4)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_simple_framer, "test_simple_framer.xml")

