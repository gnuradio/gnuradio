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
import time


class qa_socket_pdu (gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        # Test that blocks can be created and destroyed without hanging
        port = str(random.Random().randint(0, 30000) + 10000)
        self.pdu_send = network.socket_pdu("UDP_CLIENT", "localhost", port)
        self.pdu_recv = network.socket_pdu("UDP_SERVER", "localhost", port)
        self.pdu_send = None
        self.pdu_recv = None

    def test_002(self):
        # Send a PDU through a pair of UDP sockets
        port = str(random.Random().randint(0, 30000) + 10000)
        srcdata = (0x64, 0x6f, 0x67, 0x65)
        data = pmt.init_u8vector(srcdata.__len__(), srcdata)
        pdu_msg = pmt.cons(pmt.PMT_NIL, data)

        self.pdu_source = blocks.message_strobe(pdu_msg, 500)
        self.pdu_recv = network.socket_pdu("UDP_SERVER", "localhost", port)
        self.pdu_send = network.socket_pdu("UDP_CLIENT", "localhost", port)

        self.dbg = blocks.message_debug()

        self.tb.msg_connect(self.pdu_source, "strobe", self.pdu_send, "pdus")
        self.tb.msg_connect(self.pdu_recv, "pdus", self.dbg, "store")

        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()
        self.pdu_send = None
        self.pdu_recv = None

        received = self.dbg.get_message(0)
        received_data = pmt.cdr(received)
        msg_data = []
        for i in range(4):
            msg_data.append(pmt.u8vector_ref(received_data, i))
        self.assertEqual(srcdata, tuple(msg_data))

    def test_003(self):
        # Test that block stops when interacting with streaming interface
        port = str(random.Random().randint(0, 30000) + 10000)
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
        tag_dict = {"offset": 0}
        tag_dict["key"] = pmt.intern("len")
        tag_dict["value"] = pmt.from_long(8)
        tag1 = gr.python_to_tag(tag_dict)
        tag_dict["offset"] = 8
        tag2 = gr.python_to_tag(tag_dict)
        tags = [tag1, tag2]

        src = blocks.vector_source_b(srcdata, False, 1, tags)
        ts_to_pdu = pdu.tagged_stream_to_pdu(gr.types.byte_t, "len")
        pdu_send = network.socket_pdu("UDP_CLIENT", "localhost", "4141")
        #pdu_recv = network.socket_pdu("UDP_SERVER", "localhost", port)
        pdu_to_ts = pdu.pdu_to_tagged_stream(gr.types.byte_t, "len")
        head = blocks.head(gr.sizeof_char, 10)
        sink = blocks.vector_sink_b(1)

        self.tb.connect(src, ts_to_pdu)
        self.tb.msg_connect(ts_to_pdu, "pdus", pdu_send, "pdus")
        # a UDP socket connects pdu_send to pdu_recv
        # TODO: test that the recv socket can be destroyed from downstream
        # that signals DONE. Also that we get the PDUs we sent
        #self.tb.msg_connect(pdu_recv, "pdus", pdu_to_ts, "pdus")
        #self.tb.connect(pdu_to_ts, head, sink)
        self.tb.run()

    def test_004(self):
        # Test that the TCP server can stream PDUs <= the MTU size.
        port = str(random.Random().randint(0, 30000) + 10000)
        mtu = 10000
        srcdata = tuple(x % 256 for x in range(mtu))
        data = pmt.init_u8vector(srcdata.__len__(), srcdata)
        pdu_msg = pmt.cons(pmt.PMT_NIL, data)

        self.pdu_source = blocks.message_strobe(pdu_msg, 500)
        self.pdu_send = network.socket_pdu("TCP_SERVER", "localhost", port, mtu)
        self.pdu_recv = network.socket_pdu("TCP_CLIENT", "localhost", port, mtu)
        self.pdu_sink = blocks.message_debug()

        self.tb.msg_connect(self.pdu_source, "strobe", self.pdu_send, "pdus")
        self.tb.msg_connect(self.pdu_recv, "pdus", self.pdu_sink, "store")

        self.tb.start()
        time.sleep(1)
        self.tb.stop()
        self.tb.wait()

        received = self.pdu_sink.get_message(0)
        received_data = pmt.cdr(received)
        msg_data = []
        for i in range(mtu):
            msg_data.append(pmt.u8vector_ref(received_data, i))
        self.assertEqual(srcdata, tuple(msg_data))


if __name__ == '__main__':
    gr_unittest.run(qa_socket_pdu)
