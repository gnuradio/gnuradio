#!/usr/bin/env python3
#
# Copyright 2022 Josh Morman
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, streamops, blocks


class test_size_adapter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_stream_adapter_vector_stream(self):
        src_len = 1024*100
        src_data = list(range(src_len))
        expected_results = src_data

        src = blocks.vector_source_i(src_data)
        op = streamops.size_adapter()
        dst = blocks.vector_sink_i(1024)

        self.tb.connect([src, op, dst])

        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_stream_adapter_stream_vector(self):
        src_len = 1024*100
        src_data = list(range(src_len))
        expected_results = src_data

        src = blocks.vector_source_i(src_data, False, 1024)
        op = streamops.size_adapter()
        dst = blocks.vector_sink_i()

        self.tb.connect([src, op, dst])

        self.tb.run()
        self.assertEqual(expected_results, dst.data())


if __name__ == '__main__':
    gr_unittest.run(test_size_adapter)
