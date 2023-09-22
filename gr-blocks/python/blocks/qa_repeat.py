#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
# Copyright 2023 Daniel Estevez <daniel@destevez.net>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import time

from gnuradio import gr, gr_unittest, blocks, pdu
import numpy as np
import pmt


class test_repeat(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def run_fg(self, N, r, msg=""):
        src_data = np.arange(N)
        src = blocks.vector_source_i(src_data)
        rpt = blocks.repeat(gr.sizeof_int, r)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, rpt, dst)
        self.tb.run()
        self.assertTrue(np.array_equal(dst.data(), src_data.repeat(r)), msg)

    def test_001_various_sizes(self):
        sizes = ((3**10 + 2, 3), (3**6, 1), (4, 5), (10**6, 1), (10 * 2, 10 * 3))
        for size, repetitions in sizes:
            self.run_fg(size, repetitions, f"N = {size}, r = {repetitions}: not equal")

    def test_tag_propagation(self):
        N = 1000
        r = 17
        tags_in = [
            gr.tag_utils.python_to_tag((
                j, pmt.intern("test_tag"), pmt.from_long(j), pmt.PMT_NIL))
            for j in range(N)
        ]
        src = blocks.vector_source_i([0] * N, tags=tags_in)
        rpt = blocks.repeat(gr.sizeof_int, r)
        dst = blocks.vector_sink_i()
        self.tb.connect(src, rpt, dst)
        self.tb.run()
        tags_out = dst.tags()
        self.assertEqual(len(tags_in), len(tags_out))
        for t_in, t_out in zip(tags_in, tags_out):
            self.assertEqual(t_out.offset, r * t_in.offset)
            self.assertEqual(t_in.key, t_out.key)
            self.assertEqual(t_in.value, t_out.value)

    def test_tag_propagation_rate_change(self):
        # This uses a pdu_to_tagged_stream block to control when data is
        # produced, allowing us to change the repetition rate exactly when we
        # want.
        N1 = 1000
        r1 = 17
        N2 = 2000
        r2 = 13
        tags_in = [
            gr.tag_utils.python_to_tag((
                j, pmt.intern("test_tag"), pmt.from_long(j), pmt.PMT_NIL))
            for j in range(N1 + N2)
        ]
        src_tags = blocks.vector_source_b([0] * (N1 + N2), tags=tags_in)
        src_data = pdu.pdu_to_tagged_stream(gr.types.byte_t)
        tag_gate = blocks.tag_gate(gr.sizeof_char)
        tag_share = blocks.tag_share(gr.sizeof_char, gr.sizeof_char)
        rpt = blocks.repeat(gr.sizeof_char, r1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src_data, tag_gate, tag_share, rpt, dst)
        self.tb.connect(src_tags, (tag_share, 1))
        self.tb.start()
        data_pdu = pmt.cons(pmt.PMT_NIL, pmt.make_u8vector(N1, 0))
        src_data.to_basic_block()._post(pmt.intern("pdus"), data_pdu)
        # Wait for some time for the data to flow through
        time.sleep(0.1)
        rpt.set_interpolation(r2)
        data_pdu = pmt.cons(pmt.PMT_NIL, pmt.make_u8vector(N2, 0))
        src_data.to_basic_block()._post(pmt.intern("pdus"), data_pdu)
        src_data.to_basic_block()._post(
            pmt.intern('system'),
            pmt.cons(pmt.intern('done'), pmt.from_long(1)))
        self.tb.wait()
        self.assertEqual(len(dst.data()), N1 * r1 + N2 * r2)
        tags_out = dst.tags()
        self.assertEqual(len(tags_out), len(tags_in))
        for t_in, t_out in zip(tags_in[:N1], tags_out[:N1]):
            self.assertEqual(t_out.offset, r1 * t_in.offset)
            self.assertEqual(t_in.key, t_out.key)
            self.assertEqual(t_in.value, t_out.value)
        for t_in, t_out in zip(tags_in[N1:], tags_out[N1:]):
            self.assertEqual(t_out.offset, N1 * r1 + r2 * (t_in.offset - N1))
            self.assertEqual(t_in.key, t_out.key)
            self.assertEqual(t_in.value, t_out.value)


if __name__ == "__main__":
    gr_unittest.run(test_repeat)
