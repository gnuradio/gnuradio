#!/usr/bin/env python3
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, streamops
import pmtf


class test_delay(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.flowgraph()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        delta_t = 0
        tb = self.tb
        src_data = [float(x) for x in range(0, 100)]
        expected_result = list(delta_t * [0.0] + src_data)

        src = blocks.vector_source_f(src_data)
        op = streamops.delay(delta_t)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_010(self):
        delta_t = 3
        tb = self.tb
        src_data = [1.0+float(x) for x in range(0, 100)]
        expected_result = list(delta_t * [0.0] + src_data)

        src = blocks.vector_source_f(src_data)
        op = streamops.delay(delta_t)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)


    def test_011(self):
        delta_t = 3
        tb = self.tb
        src_data = [1.0+float(x) for x in range(0, 100)]
        expected_result = list(delta_t * [0.0] + src_data)

        src = blocks.vector_source_f(src_data)
        op = streamops.delay(0)
        op.set_dly(delta_t)
        dst = blocks.vector_sink_f()

        tb.connect(src, op)
        tb.connect(op, dst)
        tb.run()
        dst_data = dst.data()
        self.assertEqual(expected_result, dst_data)

    def test_020(self):
        nsamps = 123
        tb = self.tb
        vector_sink = blocks.vector_sink_f(1)
        ref_sink = blocks.vector_sink_f(1)
        tags_strobe = blocks.tags_strobe(
            pmtf.pmt("TEST"),
            nsamps,
            "strobe",
            gr.sizeof_float)
        head = streamops.head(10**5)
        delay = streamops.delay(100)
        tb.connect((delay, 0), (head, 0))
        tb.connect((head, 0), (vector_sink, 0))
        tb.connect((tags_strobe, 0), (delay, 0))
        tb.connect((tags_strobe, 0), (ref_sink, 0))
        tb.run()

        tags = vector_sink.tags()
        self.assertNotEqual(len(tags), 0)
        lastoffset = tags[0].offset() - nsamps
        for tag in tags:
            newoffset = tag.offset()
            self.assertEqual(newoffset, lastoffset + nsamps)
            lastoffset = newoffset


if __name__ == '__main__':
    gr_unittest.run(test_delay)
