#!/usr/bin/env python
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

from gnuradio import gr, gr_unittest, blocks, digital
import pmt

class qa_crc32_bb (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_crc_len (self):
        """ Make sure the output of a CRC set is 4 bytes longer than the input. """
        data = range(16)
        tag_name = "len"
        tag = gr.tag_t()
        tag.offset = 0
        tag.key = pmt.string_to_symbol(tag_name)
        tag.value = pmt.from_long(len(data))
        src = blocks.vector_source_b(data, False, 1, (tag,))
        crc = digital.crc32_bb(False, tag_name)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, crc, sink)
        self.tb.run()
        # Check that the packets before crc_check are 4 bytes longer that the input.
        self.assertEqual(len(data)+4, len(sink.data()))

    def test_002_crc_equal (self):
        """ Go through CRC set / CRC check and make sure the output
        is the same as the input. """
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8)
        tag_name = "len"
        tag = gr.tag_t()
        tag.offset = 0
        tag.key = pmt.string_to_symbol(tag_name)
        tag.value = pmt.from_long(len(data))
        src = blocks.vector_source_b(data, False, 1, (tag,))
        crc = digital.crc32_bb(False, tag_name)
        crc_check = digital.crc32_bb(True, tag_name)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, crc, crc_check, sink)
        self.tb.run()
        # Check that the packets after crc_check are the same as input.
        self.assertEqual(data, sink.data())

    def test_003_crc_correct_lentag (self):
        tag_name = "length"
        pack_len = 8
        packets = range(pack_len*2)
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol(tag_name)
        tag1.value = pmt.from_long(pack_len)
        tag2 = gr.tag_t()
        tag2.offset = pack_len
        tag2.key = pmt.string_to_symbol(tag_name)
        tag2.value = pmt.from_long(pack_len)
        testtag1 = gr.tag_t()
        testtag1.offset = 1
        testtag1.key = pmt.string_to_symbol("tag1")
        testtag1.value = pmt.from_long(0)
        testtag2 = gr.tag_t()
        testtag2.offset = pack_len
        testtag2.key = pmt.string_to_symbol("tag2")
        testtag2.value = pmt.from_long(0)
        testtag3 = gr.tag_t()
        testtag3.offset = len(packets)-1
        testtag3.key = pmt.string_to_symbol("tag3")
        testtag3.value = pmt.from_long(0)
        src = blocks.vector_source_b(packets, False, 1, (tag1, tag2, testtag1, testtag2, testtag3))
        crc = digital.crc32_bb(False, tag_name)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, crc, sink)
        self.tb.run()
        self.assertEqual(len(sink.data()), 2*(pack_len+4))
        correct_offsets = {'tag1': 1, 'tag2': 12, 'tag3': 19}
        tags_found = {'tag1': False, 'tag2': False, 'tag3': False}
        for tag in sink.tags():
            key = pmt.symbol_to_string(tag.key)
            if key in correct_offsets.keys():
                tags_found[key] = True
                self.assertEqual(correct_offsets[key], tag.offset)
            if key == tag_name:
                self.assertTrue(tag.offset == 0 or tag.offset == pack_len+4)
        self.assertTrue(all(tags_found.values()))


    def test_004_fail (self):
        """ Corrupt the data and make sure it fails CRC test. """
        data = (0, 1, 2, 3, 4, 5, 6, 7)
        tag_name = "len"
        tag = gr.tag_t()
        tag.offset = 0
        tag.key = pmt.string_to_symbol(tag_name)
        tag.value = pmt.from_long(len(data))
        src = blocks.vector_source_b(data, False, 1, (tag,))
        crc = digital.crc32_bb(False, tag_name)
        crc_check = digital.crc32_bb(True, tag_name)
        corruptor = blocks.add_const_bb(1)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, crc, corruptor, crc_check, sink)
        self.tb.run()
        # crc_check will drop invalid packets
        self.assertEqual(len(sink.data()), 0)

    def test_005_tag_propagation (self):
        """ Make sure tags on the CRC aren't lost. """
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8, 2, 67, 225, 188)
        tag_name = "len"
        tag = gr.tag_t()
        tag.offset = 0
        tag.key = pmt.string_to_symbol(tag_name)
        tag.value = pmt.from_long(len(data))
        testtag = gr.tag_t()
        testtag.offset = len(data)-1
        testtag.key = pmt.string_to_symbol('tag1')
        testtag.value = pmt.from_long(0)
        src = blocks.vector_source_b(data, False, 1, (tag, testtag))
        crc_check = digital.crc32_bb(True, tag_name)
        sink = blocks.vector_sink_b()
        self.tb.connect(src, crc_check, sink)
        self.tb.run()
        self.assertEqual([len(data)-5,], [tag.offset for tag in sink.tags() if pmt.symbol_to_string(tag.key) == 'tag1'])

if __name__ == '__main__':
    gr_unittest.run(qa_crc32_bb, "qa_crc32_bb.xml")
