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

import random
from gnuradio import gr, gr_unittest, blocks
import pmt

class qa_repack_bits_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_simple (self):
        """ Very simple test, 2 bits -> 1 """
        src_data = (0b11, 0b01, 0b10)
        expected_data = (0b1, 0b1, 0b1, 0b0, 0b0, 0b1)
        k = 2
        l = 1
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_data)

    def test_002_three (self):
        """ 8 -> 3 """
        src_data = (0b11111101, 0b11111111, 0b11111111)
        expected_data = (0b101,) + (0b111,) * 7
        k = 8
        l = 3
        src = blocks.vector_source_b(src_data, False, 1)
        repack = blocks.repack_bits_bb(k, l)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_data)

    def test_003_lots_of_bytes (self):
        """ Lots and lots of bytes, multiple packer stages """
        src_data = tuple([random.randint(0, 255) for x in range(3*5*7*8 * 10)])
        src = blocks.vector_source_b(src_data, False, 1)
        repack1 = blocks.repack_bits_bb(8, 3)
        repack2 = blocks.repack_bits_bb(3, 5)
        repack3 = blocks.repack_bits_bb(5, 7)
        repack4 = blocks.repack_bits_bb(7, 8)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack1, repack2, repack3, repack4, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), src_data)

    def test_004_three_with_tags (self):
        """ 8 -> 3 """
        src_data = (0b11111101, 0b11111111)
        expected_data = (0b101,) + (0b111,) * 4 + (0b001,)
        k = 8
        l = 3
        tag_name = "len"
        tag = gr.tag_t()
        tag.offset = 0
        tag.key = pmt.string_to_symbol(tag_name)
        tag.value = pmt.from_long(len(src_data))
        src = blocks.vector_source_b(src_data, False, 1, (tag,))
        repack = blocks.repack_bits_bb(k, l, tag_name)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_data)
        try:
            out_tag = sink.tags()[0]
        except:
            self.assertFail()
        self.assertEqual(out_tag.offset, 0)
        self.assertEqual(pmt.symbol_to_string(out_tag.key), tag_name)
        self.assertEqual(pmt.to_long(out_tag.value), len(expected_data))

    def test_005_three_with_tags_trailing (self):
        """ 3 -> 8, trailing bits """
        src_data = (0b101,) + (0b111,) * 4 + (0b001,)
        expected_data = (0b11111101, 0b11111111)
        k = 3
        l = 8
        tag_name = "len"
        tag = gr.tag_t()
        tag.offset = 0
        tag.key = pmt.string_to_symbol(tag_name)
        tag.value = pmt.from_long(len(src_data))
        src = blocks.vector_source_b(src_data, False, 1, (tag,))
        repack = blocks.repack_bits_bb(k, l, tag_name, True)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_data)
        try:
            out_tag = sink.tags()[0]
        except:
            self.assertFail()
        self.assertEqual(out_tag.offset, 0)
        self.assertEqual(pmt.symbol_to_string(out_tag.key), tag_name)
        self.assertEqual(pmt.to_long(out_tag.value), len(expected_data))

if __name__ == '__main__':
    gr_unittest.run(qa_repack_bits_bb, "qa_repack_bits_bb.xml")

