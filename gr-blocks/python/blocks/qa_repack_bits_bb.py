#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import random
from gnuradio import gr, gr_unittest, blocks
import pmt


class qa_repack_bits_bb (gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()
        self.tsb_key = "length"

    def tearDown(self):
        self.tb = None

    def test_001_simple(self):
        """ Very simple test, 2 bits -> 1 """
        src_data = [0b11, 0b01, 0b10]
        expected_data = [0b1, 0b1, 0b1, 0b0, 0b0, 0b1]
        k = 2
        l = 1
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_data)

    def test_001_simple_msb(self):
        """ Very simple test, 2 bits -> 1 with MSB set """
        src_data = [0b11, 0b01, 0b10]
        expected_data = [0b1, 0b1, 0b0, 0b1, 0b1, 0b0]
        k = 2
        l = 1
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l, "", False, gr.GR_MSB_FIRST)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_data)

    def test_002_three(self):
        """ 8 -> 3 """
        src_data = [0b11111101, 0b11111111, 0b11111111]
        expected_data = [0b101, ] + [0b111, ] * 7
        k = 8
        l = 3
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_data)

    def test_002_three_msb(self):
        """ 8 -> 3 """
        src_data = [0b11111101, 0b11111111, 0b11111111]
        expected_data = [0b111, ] + [0b111, ] + [0b011, ] + [0b111, ] * 5
        k = 8
        l = 3
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l, "", False, gr.GR_MSB_FIRST)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run()
        self.assertEqual(sink.data(), expected_data)

    def test_003_lots_of_bytes(self):
        """ Lots and lots of bytes, multiple packer stages """
        src_data = [random.randint(0, 255) for x in range(3 * 5 * 7 * 8 * 10)]
        src = blocks.vector_source_b(src_data, False, 1)
        repack1 = blocks.repack_bits_bb(8, 3)
        repack2 = blocks.repack_bits_bb(3, 5)
        repack3 = blocks.repack_bits_bb(5, 7)
        repack4 = blocks.repack_bits_bb(7, 8)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack1, repack2, repack3, repack4, sink)
        self.tb.run()
        self.assertEqual(sink.data(), src_data)

    def test_003_lots_of_bytes_msb(self):
        """ Lots and lots of bytes, multiple packer stages """
        src_data = [random.randint(0, 255) for x in range(3 * 5 * 7 * 8 * 10)]
        src = blocks.vector_source_b(src_data, False, 1)
        repack1 = blocks.repack_bits_bb(8, 3, "", False, gr.GR_MSB_FIRST)
        repack2 = blocks.repack_bits_bb(3, 5, "", False, gr.GR_MSB_FIRST)
        repack3 = blocks.repack_bits_bb(5, 7, "", False, gr.GR_MSB_FIRST)
        repack4 = blocks.repack_bits_bb(7, 8, "", False, gr.GR_MSB_FIRST)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack1, repack2, repack3, repack4, sink)
        self.tb.run()
        self.assertEqual(sink.data(), src_data)

    def test_004_three_with_tags(self):
        """ 8 -> 3 """
        src_data = [0b11111101, 0b11111111]
        expected_data = [0b101, ] + [0b111, ] * 4 + [0b001, ]
        k = 8
        l = 3
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l, self.tsb_key)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(
                gr.sizeof_char,
                1,
                len(src_data),
                self.tsb_key),
            repack,
            sink)
        self.tb.run()
        self.assertEqual(len(sink.data()), 1)
        self.assertEqual(sink.data()[0], expected_data)

    def test_005_three_with_tags_trailing(self):
        """ 3 -> 8, trailing bits """
        src_data = [0b101, ] + [0b111, ] * 4 + [0b001, ]
        expected_data = [0b11111101, 0b11111111]
        k = 3
        l = 8
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l, self.tsb_key, True)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(
                gr.sizeof_char,
                1,
                len(src_data),
                self.tsb_key),
            repack,
            sink)
        self.tb.run()
        self.assertEqual(len(sink.data()), 1)
        self.assertEqual(sink.data()[0], expected_data)


if __name__ == '__main__':
    gr_unittest.run(qa_repack_bits_bb)
