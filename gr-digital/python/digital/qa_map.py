#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# SPDX-License-Identifier: GPL-3.0-or-later
#
# 


from gnuradio import gr, gr_unittest, digital, blocks

class test_map(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def helper(self, symbols):
        src_data = [0, 1, 2, 3, 0, 1, 2, 3]
        expected_data = [symbols[x] for x in src_data]
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
        
