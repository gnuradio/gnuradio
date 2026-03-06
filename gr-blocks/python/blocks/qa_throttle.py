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

# QA tests for the GNU Radio throttle block.
#
# The throttle block regulates the processing rate of flowgraph when
# no hardware is present.These tests verify that the block:
# - correctly limits data rate according to the configured sample rate.
# - respects runtime parameters such as stream tags and chunk limits.
#
class test_throttle(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None
#
# This test verifies that the throttle block acts on the configured
# the sample rate by measuring the execution time of a flowgraph.
# The runtime should roughly match the expected processing duration.
#
    def test_throttling(self):
        src_data = list(range(200))
        src = blocks.vector_source_c(src_data)
        thr = blocks.throttle(gr.sizeof_gr_complex, 100)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        start_time = time.perf_counter()
        self.tb.run()
        end_time = time.perf_counter()

        total_time = end_time - start_time
        self.assertGreater(total_time, 1.99)
        self.assertLess(total_time, 3.0)

        dst_data = dst.data()
        self.assertEqual(src_data, dst_data)
#
# This test verifies that the throttle block correctly respects the 'rx_rate' stream tag.
# If the tag is present it overrides the configured throttle rate and controls the
# effective data rate of the flowgraph.
#
    def test_rx_rate_tag(self):
        src_data = list(range(400))
        tag = gr.tag_t()
        tag.key = pmt.string_to_symbol("rx_rate")
        tag.value = pmt.to_pmt(200)
        tag.offset = 0

        src = blocks.vector_source_c(src_data, tags=(tag,))
        thr = blocks.throttle(gr.sizeof_gr_complex, 100, ignore_tags=False)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        start_time = time.perf_counter()
        self.tb.run()
        end_time = time.perf_counter()

        total_time = end_time - start_time
        self.assertGreater(total_time, 1.99)
        self.assertLess(total_time, 3.0)

        dst_data = dst.data()
        self.assertEqual(src_data, dst_data)
#
# This test verifies that the throttle block respects the maximum_items_per_chunk parameter.
# It ensures that data is processed in limited chunk sizes during runtime.
#
    def test_limited_chunk_throttling(self):
        src_data = [1, 2, 3]
        rate = 10
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 2.0  # seconds
        start_time = time.perf_counter()
        previous_len = 0

        self.tb.start()

        while time.perf_counter() < start_time + total_time:
            time.sleep(0.1)
            current_len = len(dst.data())
            self.assertLessEqual(current_len, previous_len + chunksize)
            previous_len = current_len

        self.tb.stop()

        self.assertGreaterEqual(current_len, chunksize)


if __name__ == '__main__':
    gr_unittest.run(test_throttle)
