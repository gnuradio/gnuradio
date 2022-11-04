#!/usr/bin/env python
#
# Copyright 2012-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import math


class test_vector_insert(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [float(x) for x in range(16)]
        expected_result = tuple(src_data)

        period = 9177
        offset = 0

        src = blocks.null_source(1)
        head = blocks.head(1, 10000000)
        ins = blocks.vector_insert_b([1], period, offset)
        dst = blocks.vector_sink_b()

        self.tb.connect(src, head, ins, dst)
        self.tb.run()
        result_data = dst.data()

        for i in range(10000):
            if(i % period == offset):
                self.assertEqual(1, result_data[i])
            else:
                self.assertEqual(0, result_data[i])

    def test_002(self):  # insert tags and check their propagation, zero offset
        period = 11000
        offset = 0
        insert = [1.0, ] * 1000

        src = blocks.null_source(gr.sizeof_float)
        s2ts = blocks.stream_to_tagged_stream(
            gr.sizeof_float, 1, period - len(insert), "packet")
        head = blocks.head(gr.sizeof_float, 1000000)
        ins = blocks.vector_insert_f(insert, period, offset)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, s2ts, head, ins, dst)
        self.tb.run()

        expected_result = (1000, 12000, 23000, 34000, 45000, 56000, 67000)
        tags = dst.tags()
        offsets = [tag.offset for tag in tags]
        for i in range(len(expected_result)):
            self.assertEqual(expected_result[i], offsets[i])

    def test_003(self):  # insert tags and check their propagation, non-zero offset
        period = 11000
        offset = 1000
        insert = [1.0, ] * 1000

        src = blocks.null_source(gr.sizeof_float)
        s2ts = blocks.stream_to_tagged_stream(
            gr.sizeof_float, 1, period - len(insert), "packet")
        head = blocks.head(gr.sizeof_float, 1000000)
        ins = blocks.vector_insert_f(insert, period, offset)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, s2ts, head, ins, dst)
        self.tb.run()

        expected_result = (0, 11000, 22000, 33000, 44000, 55000, 66000)
        tags = dst.tags()
        offsets = [tag.offset for tag in tags]
        for i in range(len(expected_result)):
            self.assertEqual(expected_result[i], offsets[i])

    def test_004(self):  # insert tags and check their propagation, non-zero offset, multiple tags per copy region
        period = 11000
        offset = 1000
        packetlen = 2000
        insert = [1.0, ] * 1000

        src = blocks.null_source(gr.sizeof_float)
        s2ts = blocks.stream_to_tagged_stream(
            gr.sizeof_float, 1, packetlen, "packet")
        head = blocks.head(gr.sizeof_float, 1000000)
        ins = blocks.vector_insert_f(insert, period, offset)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, s2ts, head, ins, dst)
        self.tb.run()

        expected_result = (
            0,
            2000,
            4000,
            6000,
            8000,
            11000,
            13000,
            15000,
            17000,
            19000,
            22000,
            24000,
            26000)
        tags = dst.tags()
        offsets = [tag.offset for tag in tags]
        for i in range(len(expected_result)):
            self.assertEqual(expected_result[i], offsets[i])


if __name__ == '__main__':
    gr_unittest.run(test_vector_insert)
