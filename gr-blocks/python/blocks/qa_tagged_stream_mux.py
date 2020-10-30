#!/usr/bin/env python
#
# Copyright 2013-2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy
import pmt
from gnuradio import gr, gr_unittest, blocks
from gnuradio.gr import packet_utils


def make_tag(key, value, offset, srcid=None):
    tag = gr.tag_t()
    tag.key = pmt.string_to_symbol(key)
    tag.value = pmt.to_pmt(value)
    tag.offset = offset
    if srcid is not None:
        tag.srcid = pmt.to_pmt(srcid)
    return tag


class qa_tagged_stream_mux (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.tsb_key = "tsb_key"

    def tearDown(self):
        self.tb = None

    def setup_data_tags(self, data):
        return packet_utils.packets_to_vectors(
            data,
            self.tsb_key
        )

    def test_1(self):
        packets0 = (
            (0, 1, 2), (5, 6), (10,), (14, 15, 16,)
        )
        packets1 = (
            (3, 4), (7, 8, 9), (11, 12, 13), (17,)
        )
        expected = [[0, 1, 2, 3, 4], [5, 6, 7, 8, 9],
                    [10, 11, 12, 13], [14, 15, 16, 17]]
        data0, tags0 = self.setup_data_tags(packets0)
        data1, tags1 = self.setup_data_tags(packets1)
        tags0.append(make_tag('spam', 42, 4))
        tags1.append(make_tag('eggs', 23, 3))
        src0 = blocks.vector_source_b(data0, tags=tags0)
        src1 = blocks.vector_source_b(data1, tags=tags1)
        tagged_stream_mux = blocks.tagged_stream_mux(
            gr.sizeof_char, self.tsb_key)
        snk = blocks.tsb_vector_sink_b(tsb_key=self.tsb_key)
        self.tb.connect(src0, (tagged_stream_mux, 0))
        self.tb.connect(src1, (tagged_stream_mux, 1))
        self.tb.connect(tagged_stream_mux, snk)
        self.tb.run()
        # Check
        self.assertEqual(expected, snk.data())
        tags = [gr.tag_to_python(x) for x in snk.tags()]
        tags = sorted([(x.offset, x.key, x.value) for x in tags])
        tags_expected = [
            (6, 'spam', 42),
            (8, 'eggs', 23),
        ]
        self.assertEqual(tags, tags_expected)

    def test_preserve_tag_head_pos(self):
        """ Test the 'preserve head position' function.
        This will add a 'special' tag to item 0 on stream 1.
        It should be on item 0 of the output stream. """
        packet_len_0 = 5
        data0 = list(range(packet_len_0))
        packet_len_1 = 3
        data1 = list(range(packet_len_1))
        mux = blocks.tagged_stream_mux(
            gr.sizeof_float,
            self.tsb_key,
            1  # Mark port 1 as carrying special tags on the head position
        )
        sink = blocks.tsb_vector_sink_f(tsb_key=self.tsb_key)
        self.tb.connect(blocks.vector_source_f(data0), blocks.stream_to_tagged_stream(
            gr.sizeof_float, 1, packet_len_0, self.tsb_key), (mux, 0))
        self.tb.connect(
            blocks.vector_source_f(
                list(
                    range(packet_len_1)), tags=(
                    make_tag(
                        'spam', 'eggs', 0),)), blocks.stream_to_tagged_stream(
                gr.sizeof_float, 1, packet_len_1, self.tsb_key), (mux, 1))
        self.tb.connect(mux, sink)
        self.tb.run()
        self.assertEqual(len(sink.data()), 1)
        self.assertEqual(sink.data()[0], data0 + data1)
        self.assertEqual(len(sink.tags()), 1)
        tag = gr.tag_to_python(sink.tags()[0])
        tag = (tag.offset, tag.key, tag.value)
        tag_expected = (0, 'spam', 'eggs')
        self.assertEqual(tag, tag_expected)


if __name__ == '__main__':
    gr_unittest.run(qa_tagged_stream_mux)
