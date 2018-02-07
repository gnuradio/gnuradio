#!/usr/bin/env python
# Copyright 2012-2014 Free Software Foundation, Inc.
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


class qa_crc32_bb(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        self.tsb_key = "length"

    def tearDown(self):
        self.tb = None

    def test_001_crc_len(self):
        """ Make sure the output of a CRC set is 4 bytes longer than the input. """
        data = range(16)
        src = blocks.vector_source_b(data)
        crc = digital.crc32_bb(False, self.tsb_key)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(gr.sizeof_char, 1,
                                           len(data), self.tsb_key), crc, sink)
        self.tb.run()
        # Check that the packets before crc_check are 4 bytes longer that the input.
        self.assertEqual(len(data) + 4, len(sink.data()[0]))

    def test_002_crc_equal(self):
        """ Go through CRC set / CRC check and make sure the output
        is the same as the input. """
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8)
        src = blocks.vector_source_b(data)
        crc = digital.crc32_bb(False, self.tsb_key)
        crc_check = digital.crc32_bb(True, self.tsb_key)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key), crc,
                        crc_check, sink)
        self.tb.run()
        # Check that the packets after crc_check are the same as input.
        self.assertEqual(data, sink.data()[0])

    def test_003_crc_correct_lentag(self):
        tag_name = "length"
        pack_len = 8
        packets = range(pack_len * 2)
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
        testtag3.offset = len(packets) - 1
        testtag3.key = pmt.string_to_symbol("tag3")
        testtag3.value = pmt.from_long(0)
        src = blocks.vector_source_b(packets, False, 1,
                                     (testtag1, testtag2, testtag3))
        crc = digital.crc32_bb(False, self.tsb_key)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, pack_len, self.tsb_key), crc,
                        sink)
        self.tb.run()
        self.assertEqual(len(sink.data()), 2)
        self.assertEqual(len(sink.data()[0]), (pack_len + 4))
        self.assertEqual(len(sink.data()[1]), (pack_len + 4))
        correct_offsets = {'tag1': 1, 'tag2': 12, 'tag3': 19}
        tags_found = {'tag1': False, 'tag2': False, 'tag3': False}
        for tag in sink.tags():
            key = pmt.symbol_to_string(tag.key)
            if key in correct_offsets.keys():
                tags_found[key] = True
                self.assertEqual(correct_offsets[key], tag.offset)
        self.assertTrue(all(tags_found.values()))

    def test_004_fail(self):
        """ Corrupt the data and make sure it fails CRC test. """
        data = (0, 1, 2, 3, 4, 5, 6, 7)
        src = blocks.vector_source_b(data)
        crc = digital.crc32_bb(False, self.tsb_key)
        crc_check = digital.crc32_bb(True, self.tsb_key)
        corruptor = blocks.add_const_bb(1)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key), crc,
                        corruptor, crc_check, sink)
        self.tb.run()
        # crc_check will drop invalid packets
        self.assertEqual(len(sink.data()), 0)

    def test_005_tag_propagation(self):
        """ Make sure tags on the CRC aren't lost. """
        # Data with precalculated CRC
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8, 2, 67, 225, 188)
        testtag = gr.tag_t()
        testtag.offset = len(data) - 1
        testtag.key = pmt.string_to_symbol('tag1')
        testtag.value = pmt.from_long(0)
        src = blocks.vector_source_b(data, False, 1, (testtag, ))
        crc_check = digital.crc32_bb(True, self.tsb_key)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key),
                        crc_check, sink)
        self.tb.run()
        self.assertEqual([len(data) - 5, ], [
            tag.offset for tag in sink.tags()
            if pmt.symbol_to_string(tag.key) == 'tag1'
        ])

    # NOTE: What follows are the same tests as before but with the packed flag set to False

    def test_006_crc_len(self):
        """ Make sure the output of a CRC set is 32 (unpacked) bytes longer than the input. """
        data = range(16)
        src = blocks.vector_source_b(data)
        crc = digital.crc32_bb(False, self.tsb_key, False)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(
            src,
            blocks.stream_to_tagged_stream(gr.sizeof_char, 1,
                                           len(data), self.tsb_key), crc, sink)
        self.tb.run()
        # Check that the packets before crc_check are 4 bytes longer that the input.
        self.assertEqual(len(data) + 32, len(sink.data()[0]))

    def test_007_crc_equal(self):
        """ Go through CRC set / CRC check and make sure the output
        is the same as the input. """
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8)
        src = blocks.vector_source_b(data)
        crc = digital.crc32_bb(False, self.tsb_key, False)
        crc_check = digital.crc32_bb(True, self.tsb_key, False)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key), crc,
                        crc_check, sink)
        self.tb.run()
        # Check that the packets after crc_check are the same as input.
        self.assertEqual(data, sink.data()[0])

    def test_002_crc_equal_unpacked(self):
        """ Test unpacked operation with packed operation
        """
        data = (0, 1, 2, 3, 4, 5, 6, 7, 8)
        src = blocks.vector_source_b(data)
        unpack1 = blocks.repack_bits_bb(8, 1, self.tsb_key, False,
                                        gr.GR_LSB_FIRST)
        unpack2 = blocks.repack_bits_bb(8, 1, self.tsb_key, False,
                                        gr.GR_LSB_FIRST)
        crc_unpacked = digital.crc32_bb(False, self.tsb_key, False)
        crc_packed = digital.crc32_bb(False, self.tsb_key, True)
        sink1 = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        sink2 = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)

        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key),
                        crc_packed, unpack1, sink1)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key),
                        unpack2, crc_unpacked, sink2)
        self.tb.run()
        self.assertEqual(sink1.data(), sink2.data())

    def test_003_crc_equal_unpacked(self):
        """ Test unpacked operation with packed operation
        """
        data = range(35)
        src = blocks.vector_source_b(data)
        unpack1 = blocks.repack_bits_bb(8, 1, self.tsb_key, False,
                                        gr.GR_LSB_FIRST)
        unpack2 = blocks.repack_bits_bb(8, 1, self.tsb_key, False,
                                        gr.GR_LSB_FIRST)
        crc_unpacked = digital.crc32_bb(False, self.tsb_key, False)
        crc_packed = digital.crc32_bb(False, self.tsb_key, True)
        sink1 = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        sink2 = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)

        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key),
                        crc_packed, unpack1, sink1)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key),
                        unpack2, crc_unpacked, sink2)
        self.tb.run()
        self.assertEqual(sink1.data(), sink2.data())

    def test_008_crc_correct_lentag(self):
        tag_name = "length"
        pack_len = 8
        packets = range(pack_len * 2)
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
        testtag3.offset = len(packets) - 1
        testtag3.key = pmt.string_to_symbol("tag3")
        testtag3.value = pmt.from_long(0)
        src = blocks.vector_source_b(packets, False, 1,
                                     (testtag1, testtag2, testtag3))
        crc = digital.crc32_bb(False, self.tsb_key, False)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, pack_len, self.tsb_key), crc,
                        sink)
        self.tb.run()
        self.assertEqual(len(sink.data()), 2)
        self.assertEqual(len(sink.data()[0]), (pack_len + 32))
        self.assertEqual(len(sink.data()[1]), (pack_len + 32))
        correct_offsets = {'tag1': 1, 'tag2': 8 + 32, 'tag3': 15 + 32}
        tags_found = {'tag1': False, 'tag2': False, 'tag3': False}
        for tag in sink.tags():
            key = pmt.symbol_to_string(tag.key)
            if key in correct_offsets.keys():
                tags_found[key] = True
                self.assertEqual(correct_offsets[key], tag.offset)
        self.assertTrue(all(tags_found.values()))

    def test_009_fail(self):
        """ Corrupt the data and make sure it fails CRC test. """
        data = (0, 1, 2, 3, 4, 5, 6, 7)
        src = blocks.vector_source_b(data)
        crc = digital.crc32_bb(False, self.tsb_key, False)
        crc_check = digital.crc32_bb(True, self.tsb_key, False)
        corruptor = blocks.add_const_bb(1)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key), crc,
                        corruptor, crc_check, sink)
        self.tb.run()
        # crc_check will drop invalid packets
        self.assertEqual(len(sink.data()), 0)

    def test_0010_tag_propagation(self):
        """ Make sure tags on the CRC aren't lost. """
        # Data with precalculated CRC
        data = (0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
                1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
                0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
                0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1)
        testtag = gr.tag_t()
        testtag.offset = len(data) - 1
        testtag.key = pmt.string_to_symbol('tag1')
        testtag.value = pmt.from_long(0)
        src = blocks.vector_source_b(data, False, 1, (testtag, ))
        crc_check = digital.crc32_bb(True, self.tsb_key, False)
        sink = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src,
                        blocks.stream_to_tagged_stream(
                            gr.sizeof_char, 1, len(data), self.tsb_key),
                        crc_check, sink)
        self.tb.run()
        self.assertEqual([len(data) - 33, ], [
            tag.offset for tag in sink.tags()
            if pmt.symbol_to_string(tag.key) == 'tag1'
        ])


if __name__ == '__main__':
    gr_unittest.run(qa_crc32_bb, "qa_crc32_bb.xml")
