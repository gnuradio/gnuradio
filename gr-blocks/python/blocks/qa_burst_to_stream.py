#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2023-2024 Daniel Estevez <daniel@destevez.net>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: MIT
#

from gnuradio import gr, gr_unittest, blocks
import pmt
import numpy as np


class qa_burst_to_stream(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_instance(self):
        instance = blocks.burst_to_stream(gr.sizeof_char, 'packet_len')

    def test_one_burst(self):
        burst2stream = blocks.burst_to_stream(
            gr.sizeof_float, 'packet_len', True)
        packet_len = 100
        packet = [1.0] * packet_len
        packet_len_tag = gr.tag_utils.python_to_tag((
            0, pmt.intern('packet_len'),
            pmt.from_long(packet_len), pmt.intern('src')))
        packet_source = blocks.vector_source_f(
            packet, False, 1, [packet_len_tag])
        # We collect many samples to have a chance of seeing our packet
        nsamples_collect = 2**20
        head = blocks.head(gr.sizeof_float, nsamples_collect)
        sink = blocks.vector_sink_f(1, nsamples_collect)
        self.tb.connect(packet_source, burst2stream, head, sink)

        self.tb.run()

        data = np.array(sink.data(), dtype='float32')
        self.assertEqual(set(data), {0.0, 1.0})
        packet_start = np.where(data == 1.0)[0][0]
        np.testing.assert_equal(data[packet_start:][:+packet_len], packet)

        tags = sink.tags()
        self.assertEqual(len(tags), 1)  # only one packet_len tag
        tag = tags[0]
        self.assertEqual(pmt.symbol_to_string(tag.key), 'packet_len')
        self.assertEqual(pmt.to_long(tag.value), packet_len)
        self.assertEqual(packet_start, tag.offset)


if __name__ == '__main__':
    gr_unittest.run(qa_burst_to_stream)
