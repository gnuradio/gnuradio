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
        src_data = list(range(200))
        src = blocks.vector_source_c(src_data)
        thr = blocks.throttle(gr.sizeof_gr_complex, 100)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        start_time = time.perf_counter()
        self.tb.run()
        end_time = time.perf_counter()

        total_time = end_time - start_time
        self.assertGreater(total_time, 2.0)
        self.assertLess(total_time, 3.0)

        dst_data = dst.data()
        self.assertEqual(src_data, dst_data)

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
        self.assertGreater(total_time, 2.0)
        self.assertLess(total_time, 3.0)

        dst_data = dst.data()
        self.assertEqual(src_data, dst_data)

    def test_limited_chunk_throttling_0(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_1(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_2(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_3(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_4(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_5(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_6(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_7(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_8(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_9(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)

    def test_limited_chunk_throttling_10(self):
        src_data = [1, 2, 3]
        rate = 50
        chunksize = 10
        src = blocks.vector_source_c(src_data, repeat=True)

        thr = blocks.throttle(gr.sizeof_gr_complex,
                              rate,
                              ignore_tags=True,
                              maximum_items_per_chunk=chunksize)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, thr, dst)

        total_time = 0.5  # seconds
        before = time.perf_counter()
        self.tb.start()
        time.sleep(total_time)
        self.tb.stop()
        after = time.perf_counter()
        duration = after - before
        self.assertLess(duration, total_time + 2/rate)
        self.assertGreater(duration, total_time - 2/rate)

        dst_data = dst.data()
        num = len(dst_data)
        # be at most one chunksize
        self.assertGreater(total_time * rate + chunksize + 1, num)
        self.assertLess(total_time * rate - chunksize - 1, num)


if __name__ == '__main__':
    gr_unittest.run(test_throttle)
