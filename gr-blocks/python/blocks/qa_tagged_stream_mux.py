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

from gnuradio import gr, gr_unittest, blocks
import pmt
import numpy

def make_len_tags(tupl, key):
    tags = []
    tag = gr.tag_t()
    tag.key = pmt.string_to_symbol(key)
    n_read = 0
    for element in tupl:
        tag.offset = n_read
        n_read += len(element)
        tag.value = pmt.to_pmt(len(element))
        tags.append(tag)
    return tags

def make_len_tag(offset, key, value):
    tag = gr.tag_t()
    tag.offset = offset
    tag.key = pmt.string_to_symbol(key)
    tag.value = pmt.to_pmt(value)
    return tag


class qa_tagged_stream_mux (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_1(self):
        datas = (
            0, 1, 2,  5, 6,     10,          14, 15, 16,
            3, 4,     7, 8, 9,  11, 12, 13,  17
        )
        expected = tuple(range(18))

        tagname = "packet_length"
        len_tags_0 = (
            make_len_tag(0, tagname, 3),
            make_len_tag(3, tagname, 2),
            make_len_tag(5, tagname, 1),
            make_len_tag(6, tagname, 3)
        )
        len_tags_1 = (
            make_len_tag(0, tagname, 2),
            make_len_tag(2, tagname, 3),
            make_len_tag(5, tagname, 3),
            make_len_tag(8, tagname, 1)
        )
        test_tag_0 = gr.tag_t()
        test_tag_0.key = pmt.string_to_symbol('spam')
        test_tag_0.offset = 4 # On the second '1'
        test_tag_0.value = pmt.to_pmt(42)
        test_tag_1 = gr.tag_t()
        test_tag_1.key = pmt.string_to_symbol('eggs')
        test_tag_1.offset = 3 # On the first '3' of the 2nd stream
        test_tag_1.value = pmt.to_pmt(23)

        src0 = blocks.vector_source_b(datas[0:9], False, 1, len_tags_0 + (test_tag_0,))
        src1 = blocks.vector_source_b(datas[9:],  False, 1, len_tags_1 + (test_tag_1,))
        tagged_stream_mux = blocks.tagged_stream_mux(gr.sizeof_char, tagname)
        snk = blocks.vector_sink_b()
        self.tb.connect(src0, (tagged_stream_mux, 0))
        self.tb.connect(src1, (tagged_stream_mux, 1))
        self.tb.connect(tagged_stream_mux, snk)
        self.tb.run()

        self.assertEqual(expected, snk.data())

        tags = [gr.tag_to_python(x) for x in snk.tags()]
        tags = sorted([(x.offset, x.key, x.value) for x in tags])
        tags_expected = [
                (0, 'packet_length', 5),
                (5, 'packet_length', 5),
                (6, 'spam', 42),
                (8, 'eggs', 23),
                (10, 'packet_length', 4),
                (14, 'packet_length', 4)
        ]
        self.assertEqual(tags, tags_expected)


if __name__ == '__main__':
    gr_unittest.run(qa_tagged_stream_mux, "qa_tagged_stream_mux.xml")

