#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import time
import pmt
from gnuradio import gr, gr_unittest, blocks


class test_throttle(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_throttling(self):
        src_data = (1, 2, 3)
        src = blocks.vector_source_c(src_data)
        thr = blocks.throttle(gr.sizeof_gr_complex, 10)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        start_time = time.perf_counter()
        self.tb.run()
        end_time = time.perf_counter()

        total_time = end_time - start_time
        self.assertGreater(total_time, 0.3)
        self.assertLess(total_time, 0.4)

        dst_data = dst.data()
        self.assertEqual(src_data, dst_data)

    def test_rx_rate_tag(self):
        src_data = (1, 2, 3, 4, 5, 6)
        tag = gr.tag_t()
        tag.key = pmt.string_to_symbol("rx_rate")
        tag.value = pmt.to_pmt(20)
        tag.offset = 0

        src = blocks.vector_source_c(src_data, tags=(tag,))
        thr = blocks.throttle(gr.sizeof_gr_complex, 10, ignore_tags=False)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        start_time = time.perf_counter()
        self.tb.run()
        end_time = time.perf_counter()

        total_time = end_time - start_time
        self.assertGreater(total_time, 0.3)
        self.assertLess(total_time, 0.4)

        dst_data = dst.data()
        self.assertEqual(src_data, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_throttle)
