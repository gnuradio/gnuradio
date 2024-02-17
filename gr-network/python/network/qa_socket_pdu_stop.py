#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, pdu
from gnuradio import network
import random
import pmt


class qa_socket_pdu_stop (gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_stop_streaming(self):
        # Test that block stops when interacting with streaming interface
        # port = str(random.Random().randint(0, 30000) + 10000)
        srcdata = (
            0x73,
            0x75,
            0x63,
            0x68,
            0x74,
            0x65,
            0x73,
            0x74,
            0x76,
            0x65,
            0x72,
            0x79,
            0x70,
            0x61,
            0x73,
            0x73)
        tag_dict = {"offset": 0,
                    "key": pmt.intern("len"),
                    "value": pmt.from_long(8)}
        tag1 = gr.python_to_tag(tag_dict)
        tag_dict["offset"] = 8
        tag2 = gr.python_to_tag(tag_dict)
        tags = [tag1, tag2]

        src = blocks.vector_source_b(srcdata, False, 1, tags)
        ts_to_pdu = pdu.tagged_stream_to_pdu(gr.types.byte_t, "len")
        pdu_send = network.socket_pdu("UDP_CLIENT", "localhost", "4141")
        # pdu_recv = network.socket_pdu("UDP_SERVER", "localhost", port)
        # pdu_to_ts = pdu.pdu_to_tagged_stream(gr.types.byte_t, "len")
        # head = blocks.head(gr.sizeof_char, 10)
        # sink = blocks.vector_sink_b(1)

        self.tb.connect(src, ts_to_pdu)
        self.tb.msg_connect(ts_to_pdu, "pdus", pdu_send, "pdus")
        # a UDP socket connects pdu_send to pdu_recv
        # TODO: test that the recv socket can be destroyed from downstream
        # that signals DONE. Also that we get the PDUs we sent
        # self.tb.msg_connect(pdu_recv, "pdus", pdu_to_ts, "pdus")
        # self.tb.connect(pdu_to_ts, head, sink)
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(qa_socket_pdu_stop)
