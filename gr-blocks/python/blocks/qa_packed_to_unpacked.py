#!/usr/bin/env python
#
# Copyright 2005,2007,2010,2013 Free Software Foundation, Inc.
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

import random

class test_packing(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block ()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = (0x80,)
        expected_results = (1,0,0,0,0,0,0,0)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_002(self):
        src_data = (0x80,)
        expected_results = (0,0,0,0,0,0,0,1)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.packed_to_unpacked_bb(1, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_003(self):
        src_data = (0x11,)
        expected_results = (4, 2)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.packed_to_unpacked_bb(3, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_004(self):
        src_data = (0x11,)
        expected_results = (0, 4)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.packed_to_unpacked_bb(3, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_005(self):
        src_data = (1,0,0,0,0,0,1,0,0,1,0,1,1,0,1,0)
        expected_results = (0x82, 0x5a)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_006(self):
        src_data = (0,1,0,0,0,0,0,1,0,1,0,1,1,0,1,0)
        expected_results = (0x82, 0x5a)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.unpacked_to_packed_bb(1, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_007(self):
        src_data = (4, 2, 0,0,0)
        expected_results = (0x11,)
        src = blocks.vector_source_b(src_data, False)
        op = blocks.unpacked_to_packed_bb(3, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_008(self):
        src_data = (0, 4, 2,0,0)
        expected_results = (0x11,)
        src = blocks.vector_source_b(src_data,False)
        op = blocks.unpacked_to_packed_bb(3, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_009(self):
        random.seed(0)
        src_data = []
        for i in xrange(202):
            src_data.append((random.randint(0,255)))
        src_data = tuple(src_data)
        expected_results = src_data

        src = blocks.vector_source_b(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_bb(3, gr.GR_MSB_FIRST)
        op2 = blocks.unpacked_to_packed_bb(3, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results[0:201], dst.data())

    def test_010(self):
        random.seed(0)
        src_data = []
        for i in xrange(56):
            src_data.append((random.randint(0,255)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_b(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_bb(7, gr.GR_MSB_FIRST)
        op2 = blocks.unpacked_to_packed_bb(7, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results[0:201], dst.data())

    def test_011(self):
        random.seed(0)
        src_data = []
        for i in xrange(56):
            src_data.append((random.randint(0,255)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_b(tuple(src_data),False)
        op1 = blocks.packed_to_unpacked_bb(7, gr.GR_LSB_FIRST)
        op2 = blocks.unpacked_to_packed_bb(7, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results[0:201], dst.data())


    # tests on shorts
    def test_100a(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**15,2**15-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_s(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ss(1, gr.GR_MSB_FIRST)
        op2 = blocks.unpacked_to_packed_ss(1, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_s()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_100b(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**15,2**15-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_s(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ss(1, gr.GR_LSB_FIRST)
        op2 = blocks.unpacked_to_packed_ss(1, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_s()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_101a(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**15,2**15-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_s(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ss(8, gr.GR_MSB_FIRST)
        op2 = blocks.unpacked_to_packed_ss(8, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_s()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_101b(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**15,2**15-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_s(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ss(8, gr.GR_LSB_FIRST)
        op2 = blocks.unpacked_to_packed_ss(8, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_s()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())


    # tests on ints
    def test_200a(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**31,2**31-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_i(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ii(1, gr.GR_MSB_FIRST)
        op2 = blocks.unpacked_to_packed_ii(1, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_200b(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**31,2**31-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_i(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ii(1, gr.GR_LSB_FIRST)
        op2 = blocks.unpacked_to_packed_ii(1, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_201a(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**31,2**31-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_i(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ii(8, gr.GR_MSB_FIRST)
        op2 = blocks.unpacked_to_packed_ii(8, gr.GR_MSB_FIRST)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

    def test_201b(self):
        random.seed(0)
        src_data = []
        for i in xrange(100):
            src_data.append((random.randint(-2**31,2**31-1)))
        src_data = tuple(src_data)
        expected_results = src_data
        src = blocks.vector_source_i(tuple(src_data), False)
        op1 = blocks.packed_to_unpacked_ii(8, gr.GR_LSB_FIRST)
        op2 = blocks.unpacked_to_packed_ii(8, gr.GR_LSB_FIRST)
        dst = blocks.vector_sink_i()

        self.tb.connect(src, op1, op2)
        self.tb.connect(op2, dst)
        self.tb.run()

        self.assertEqual(expected_results, dst.data())

if __name__ == '__main__':
   gr_unittest.run(test_packing, "test_packing.xml")

