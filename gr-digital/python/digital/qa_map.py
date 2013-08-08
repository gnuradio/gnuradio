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

from gnuradio import gr, gr_unittest, digital, blocks

class test_map(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def helper(self, symbols):
        src_data = [0, 1, 2, 3, 0, 1, 2, 3]
        expected_data = map(lambda x: symbols[x], src_data)
        src = blocks.vector_source_b(src_data)
        op = digital.map_bb(symbols)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()

        result_data = list(dst.data())
        self.assertEqual(expected_data, result_data)

    def test_001(self):
        symbols = [0, 0, 0, 0]
        self.helper(symbols)
        
    def test_002(self):
        symbols = [3, 2, 1, 0]
        self.helper(symbols)

    def test_003(self):
        symbols = [8-1, 32-1, 128, 256-1]
        self.helper(symbols)

if __name__ == '__main__':
    gr_unittest.run(test_map, "test_map.xml")
        
