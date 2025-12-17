#!/usr/bin/env python
#
# Copyright 2025 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
from gnuradio.blocks import vector_sink_b, vector_source_b, annotator_raw, throttle
from time import sleep
import pmt


class test_annotator_raw(gr_unittest.TestCase):

    def compare_tag_iterables(self, tags_out: list, tags_in: list, name: str):
        for tag, ref in zip(tags_out, tags_in):
            self.assertTupleEqual(
                (tag.offset, tag.key, tag.value),
                ref,
                "tag contents differ",
            )
            self.assertEqual(pmt.to_python(tag.srcid), name)

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_instantiation(self):
        blk = annotator_raw(1)
        self.assertTrue(blk)

    def test_002_preseed(self):
        N = 1000
        tags_in = [(n * N, pmt.mp(f"key_{n}"), pmt.from_long(n * 10)) for n in range(N)]

        source = vector_source_b([i % 256 for i in range(N * N)], repeat=False)
        blk = annotator_raw(gr.sizeof_char)
        sink = vector_sink_b(reserve_items=N * N)
        self.tb.connect(source, blk, sink)

        for tag_tuple in tags_in:
            blk.add_tag(*tag_tuple)

        self.tb.run()

        self.assertEqual(
            N * N, len(sink.data()), "did not get correct number of samples"
        )

        tags_out = sink.tags()
        self.compare_tag_iterables(tags_out, tags_in, blk.name())

    def test_003_late_insertion(self):
        N = 2048
        total_time = 1.0
        tags_in = [
            (n * N, pmt.mp(f"key_{n}"), pmt.from_long(n * 10)) for n in range(N // 2, N)
        ]

        source = vector_source_b([i % 256 for i in range(N * N)], repeat=False)
        slower = throttle(
            gr.sizeof_char, N * N / total_time, maximum_items_per_chunk=16
        )
        blk = annotator_raw(gr.sizeof_char)
        sink = vector_sink_b(reserve_items=N * N)
        self.tb.connect(source, slower, blk, sink)

        self.tb.start()
        sleep(0.2 * total_time)
        # we should be a fair bit into the input, but not yet halfway through
        for tag_tuple in tags_in:
            blk.add_tag(*tag_tuple)
        self.assertRaises(
            RuntimeError, lambda: blk.add_tag(0, pmt.PMT_NIL, pmt.PMT_NIL)
        )
        self.tb.wait()

        self.assertEqual(
            N * N, len(sink.data()), "did not get correct number of samples"
        )

        tags_out = sink.tags()
        self.compare_tag_iterables(tags_out, tags_in, blk.name())


if __name__ == "__main__":
    gr_unittest.run(test_annotator_raw)
