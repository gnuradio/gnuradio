#!/usr/bin/env python
#
# Copyright 2006,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import random

from gnuradio import gr, gr_unittest, blocks

class test_pack(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data =              (1,0,1,1,0,1,1,0)
        expected_results =      (1,0,1,1,0,1,1,0)
        src = blocks.vector_source_b(src_data,False)
        op = blocks.pack_k_bits_bb(1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_002(self):
        src_data =              (1,0,1,1,0,0,0,1)
        expected_results =      (  2,  3,  0,  1)
        src = blocks.vector_source_b(src_data,False)
        op = blocks.pack_k_bits_bb(2)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        #self.assertEqual(expected_results, dst.data())
        self.assertEqual(expected_results, dst.data())

    def test_003(self):
        src_data = expected_results = [random.randint(0,3) for x in range(10)];
        src = blocks.vector_source_b( src_data );
        pack = blocks.pack_k_bits_bb(2);
        unpack = blocks.unpack_k_bits_bb(2);
        snk = blocks.vector_sink_b();
        self.tb.connect(src,unpack,pack,snk);
        self.tb.run()
        self.assertEqual(list(expected_results), list(snk.data()));

if __name__ == '__main__':
   gr_unittest.run(test_pack, "test_pack.xml")

