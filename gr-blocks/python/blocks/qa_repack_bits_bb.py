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

    def test_switch_k_with_tag (self):
        """
        A: 3 -> 8 (3*9=27 -> 8*3+3)  It throws away extra bits.
        B: 5 -> 8 (5*3=15 -> 8*1+7)
        """
        src_dataA = (0b101,) + (0b111,) * 7 + (0b001,)
        src_dataB = (0b10101,) + (0b11111,) + (0b00110,)
        expected_dataA = (0b11111101, 0b11111111, 0b11111111, )
        expected_dataB = (0b11110101, )
        kA = 3
        kB = 5
        l = 8
        tag_name = "len"
        len_tagA = gr.tag_t()
        len_tagA.offset = 0
        len_tagA.key = pmt.string_to_symbol(tag_name)
        len_tagA.value = pmt.from_long(len(src_dataA))

        k_tagA = gr.tag_t()
        k_tagA.offset = 0
        k_tagA.key = pmt.string_to_symbol("set_n_input_bits")
        k_tagA.value = pmt.from_long(kA)
        len_tagB = gr.tag_t()
        len_tagB.offset = len(src_dataA)
        len_tagB.key = pmt.string_to_symbol(tag_name)
        len_tagB.value = pmt.from_long(len(src_dataB))
        k_tagB = gr.tag_t()
        k_tagB.offset = len(src_dataA)
        k_tagB.key = pmt.string_to_symbol("set_n_input_bits")
        k_tagB.value = pmt.from_long(kB)
        src = blocks.vector_source_b(src_dataA+src_dataB, False, 1,
                                     (len_tagA, k_tagA, len_tagB, k_tagB))
        repack = blocks.repack_bits_bb(kA, l, tag_name, True)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, repack, sink)
        self.tb.run ()
        self.assertEqual(sink.data(), expected_dataA+expected_dataB)
        # The output should contains 4 tags.
        # Two these should be length tags.
        length_tags = []
        for tag in sink.tags():
            if pmt.symbol_to_string(tag.key) == tag_name:
                length_tags.append(tag)
        self.assertEqual(len(length_tags), 2)
        out_tagA = length_tags[0]
        out_tagB = length_tags[1]
        self.assertEqual(out_tagA.offset, 0)
        self.assertEqual(out_tagB.offset, len(expected_dataA))
        self.assertEqual(pmt.symbol_to_string(out_tagA.key), tag_name)
        self.assertEqual(pmt.symbol_to_string(out_tagB.key), tag_name)
        self.assertEqual(pmt.to_long(out_tagA.value), len(expected_dataA))
        self.assertEqual(pmt.to_long(out_tagB.value), len(expected_dataB))

if __name__ == '__main__':
    gr_unittest.run(qa_repack_bits_bb, "qa_repack_bits_bb.xml")

