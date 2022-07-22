#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
from gnuradio import blocks, zeromq
import pmtf
import time


def make_tag(key, value, offset, srcid=None):
    
    if srcid is None:
        tag = gr.tag_t(offset, {key: pmtf.pmt(value)})
    else:
        tag = gr.tag_t(offset, {key: pmtf.pmt(value), "srcid": pmtf.pmt(srcid)})
    return tag


# def compare_tags(a, b):
#     return a.offset == b.offset and pmt.equal(a.key, b.key) and \
#         pmt.equal(a.value, b.value) and pmt.equal(a.srcid, b.srcid)


class qa_zeromq_pubsub (gr_unittest.TestCase):

    def setUp(self):
        self.send_tb = gr.top_block()
        self.recv_tb = gr.top_block()

    def tearDown(self):
        self.send_tb = None
        self.recv_tb = None

    def test_001(self):
        vlen = 10
        src_data = list(range(vlen)) * 100
        src = blocks.vector_source_f(src_data, False, vlen)
        zeromq_pub_sink = zeromq.pub_sink("tcp://127.0.0.1:0", 0)
        address = zeromq_pub_sink.last_endpoint()
        zeromq_sub_source = zeromq.sub_source(address, 0)
        sink = blocks.vector_sink_f(vlen)
        self.send_tb.connect(src, zeromq_pub_sink)
        self.recv_tb.connect(zeromq_sub_source, sink)
        self.recv_tb.start()
        time.sleep(0.5)
        self.send_tb.start()
        time.sleep(0.5)
        self.recv_tb.stop()
        self.send_tb.stop()
        # self.recv_tb.wait()
        # self.send_tb.wait()
        self.assertFloatTuplesAlmostEqual(sink.data(), src_data)

    def test_002(self):
        # same as test_001, but insert a tag and set key filter
        vlen = 10
        src_data = list(range(vlen)) * 100

        src_tags = tuple([make_tag('key', 'val', 0, 'src'),
                          make_tag('key', 'val', 1, 'src')])

        src = blocks.vector_source_f(src_data, False, vlen, tags=src_tags)
        zeromq_pub_sink = zeromq.pub_sink(
            "tcp://127.0.0.1:0",
            0,
            pass_tags=True,
            key="filter_key")
        address = zeromq_pub_sink.last_endpoint()
        zeromq_sub_source = zeromq.sub_source(address, 0, pass_tags=True, key="filter_key")
        sink = blocks.vector_sink_f(vlen)
        self.send_tb.connect(src, zeromq_pub_sink)
        self.recv_tb.connect(zeromq_sub_source, sink)

        # start both flowgraphs
        self.recv_tb.start()
        time.sleep(0.5)
        self.send_tb.start()
        time.sleep(0.5)
        self.recv_tb.stop()
        self.send_tb.stop()
        self.recv_tb.wait()
        self.send_tb.wait()

        # compare data
        self.assertFloatTuplesAlmostEqual(sink.data(), src_data)

        # compare all tags
        rx_tags = sink.tags()
        self.assertEqual(len(src_tags), len(rx_tags))

        idx = 0
        for in_tag, out_tag in zip(src_tags, rx_tags):
            print(idx)
            print(in_tag)
            print(out_tag)
            self.assertTrue(in_tag == out_tag)
            idx += 1


if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_pubsub)
