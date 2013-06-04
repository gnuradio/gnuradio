#!/usr/bin/env python
# Copyright 2012 Free Software Foundation, Inc.
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
import pmt

class qa_packet_headergenerator_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_12bits (self):
        # 3 PDUs: |           |       |
        data   = (1, 2, 3, 4, 1, 2) + tuple(range(25))
        tagname = "packet_len"
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol(tagname)
        tag1.value = pmt.from_long(4)
        tag2 = gr.tag_t()
        tag2.offset = 4
        tag2.key = pmt.string_to_symbol(tagname)
        tag2.value = pmt.from_long(2)
        tag3 = gr.tag_t()
        tag3.offset = 6
        tag3.key = pmt.string_to_symbol(tagname)
        tag3.value = pmt.from_long(25)
        src = blocks.vector_source_b(data, False, 1, (tag1, tag2, tag3))
        header = digital.packet_headergenerator_bb(12, tagname)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, header, sink)
        self.tb.run()
        expected_data = (
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0
        )
        self.assertEqual(sink.data(), expected_data)


    def test_002_32bits (self):
        # 3 PDUs: |           |     |         |
        data   = (1, 2, 3, 4, 1, 2, 1, 2, 3, 4)
        tagname = "packet_len"
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol(tagname)
        tag1.value = pmt.from_long(4)
        tag2 = gr.tag_t()
        tag2.offset = 4
        tag2.key = pmt.string_to_symbol(tagname)
        tag2.value = pmt.from_long(2)
        tag3 = gr.tag_t()
        tag3.offset = 6
        tag3.key = pmt.string_to_symbol(tagname)
        tag3.value = pmt.from_long(4)
        src = blocks.vector_source_b(data, False, 1, (tag1, tag2, tag3))
        header = digital.packet_headergenerator_bb(32, tagname)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, header, sink)
        self.tb.run()
        expected_data = (
            #   | Number of symbols                  | Packet number                                  | Parity
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0,
                0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0
        )
        self.assertEqual(sink.data(), expected_data)


    def test_003_12bits_formatter_object (self):
        # 3 PDUs: |           |     |         |
        data   = (1, 2, 3, 4, 1, 2, 1, 2, 3, 4)
        tagname = "packet_len"
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol(tagname)
        tag1.value = pmt.from_long(4)
        tag2 = gr.tag_t()
        tag2.offset = 4
        tag2.key = pmt.string_to_symbol(tagname)
        tag2.value = pmt.from_long(2)
        tag3 = gr.tag_t()
        tag3.offset = 6
        tag3.key = pmt.string_to_symbol(tagname)
        tag3.value = pmt.from_long(4)
        src = blocks.vector_source_b(data, False, 1, (tag1, tag2, tag3))
        formatter_object = digital.packet_header_default(12, tagname)
        header = digital.packet_headergenerator_bb(formatter_object.formatter(), tagname)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, header, sink)
        self.tb.run()
        expected_data = (
            0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
        )
        self.assertEqual(sink.data(), expected_data)

    def test_004_8bits_formatter_ofdm (self):
        occupied_carriers = ((1, 2, 3, 5, 6, 7),)
        # 3 PDUs: |           |     |         |
        data   = (1, 2, 3, 4, 1, 2, 1, 2, 3, 4)
        tagname = "packet_len"
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol(tagname)
        tag1.value = pmt.from_long(4)
        tag2 = gr.tag_t()
        tag2.offset = 4
        tag2.key = pmt.string_to_symbol(tagname)
        tag2.value = pmt.from_long(2)
        tag3 = gr.tag_t()
        tag3.offset = 6
        tag3.key = pmt.string_to_symbol(tagname)
        tag3.value = pmt.from_long(4)
        src = blocks.vector_source_b(data, False, 1, (tag1, tag2, tag3))
        formatter_object = digital.packet_header_ofdm(occupied_carriers, 1, tagname)
        self.assertEqual(formatter_object.header_len(), 6)
        self.assertEqual(pmt.symbol_to_string(formatter_object.len_tag_key()), tagname)
        header = digital.packet_headergenerator_bb(formatter_object.formatter(), tagname)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, header, sink)
        self.tb.run()
        expected_data = (
            0, 0, 1, 0, 0, 0,
            0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 0, 0
        )
        self.assertEqual(sink.data(), expected_data)

if __name__ == '__main__':
    gr_unittest.run(qa_packet_headergenerator_bb, "qa_packet_headergenerator_bb.xml")

