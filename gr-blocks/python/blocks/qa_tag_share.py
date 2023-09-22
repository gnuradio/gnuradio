#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt


class qa_tag_share(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        # Constants
        tag_key = 'in1_tag'
        tag_value = 0
        tag_offset = 0
        in0_value = 1.0 + 1.0j
        in1_value = 2.717
        in0_data = [in0_value, ] * 10
        in1_data = [in1_value, ] * 10
        sink_data = in0_data

        tag = gr.tag_t()
        tag.key = pmt.to_pmt(tag_key)
        tag.value = pmt.to_pmt(tag_value)
        tag.offset = tag_offset

        # Only tag Input 1 of the share block and see if it transfers
        # to Output 0. Also verify that Input 0 stream is propagated to
        # Output 0.
        in0 = blocks.vector_source_c(in0_data, False, 1)
        in1 = blocks.vector_source_f(in1_data, False, 1, (tag,))
        tag_share = blocks.tag_share(gr.sizeof_gr_complex, gr.sizeof_float)
        sink = blocks.vector_sink_c(1)

        self.tb.connect(in0, (tag_share, 0))
        self.tb.connect(in1, (tag_share, 1))
        self.tb.connect(tag_share, sink)
        self.tb.run()

        self.assertEqual(len(sink.tags()), 1)
        # print(sink.tags())
        received_tag = sink.tags()[0]
        self.assertEqual(pmt.to_python(received_tag.key), tag_key)
        self.assertEqual(pmt.to_python(received_tag.value), tag_value)
        self.assertEqual(received_tag.offset, tag_offset)
        self.assertEqual(sink.data(), sink_data)


if __name__ == '__main__':
    gr_unittest.run(qa_tag_share)
