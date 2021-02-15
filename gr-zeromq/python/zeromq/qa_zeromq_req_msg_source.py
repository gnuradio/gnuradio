#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""Unit tests for ZMQ REQ Message Source block"""

import sys
import time
import zmq

from gnuradio import gr, gr_unittest, blocks, zeromq
import pmt


class qa_zeromq_req_msg_source(gr_unittest.TestCase):
    """Unit tests for ZMQ REQ Message Source block"""

    def setUp(self):
        addr = 'tcp://127.0.0.1'
        self.context = zmq.Context()
        self.zmq_sock = self.context.socket(zmq.REP)
        port = self.zmq_sock.bind_to_random_port(addr)

        self.zeromq_req_msg_source = zeromq.req_msg_source(
            ('%s:%s' % (addr, port)), 100)
        self.message_debug = blocks.message_debug()
        self.tb = gr.top_block()
        self.tb.msg_connect(
            (self.zeromq_req_msg_source, 'out'), (self.message_debug, 'store'))

        self.tb.start()

    def tearDown(self):
        self.tb.stop()
        self.tb.wait()
        self.zeromq_req_msg_source = None
        self.message_debug = None
        self.tb = None
        self.zmq_sock.close()
        self.context.term()

    def recv_request(self):
        """Receive the request output items message"""
        req = self.zmq_sock.recv()
        req_output_items = int.from_bytes(req, sys.byteorder)
        self.assertEqual(1, req_output_items)

    def send_messages(self, msgs):
        """Send multiple messages"""
        for msg in msgs:
            self.recv_request()
            self.zmq_sock.send(pmt.serialize_str(msg))

    def test_valid_pmt(self):
        """Test receiving of valid PMT messages"""
        msg = pmt.to_pmt('test_valid_pmt')
        self.send_messages([msg])
        for _ in range(10):
            if self.message_debug.num_messages() > 0:
                break
            time.sleep(0.2)
        self.assertEqual(1, self.message_debug.num_messages())
        self.assertTrue(pmt.equal(msg, self.message_debug.get_message(0)))

    def test_multiple_messages(self):
        """Test receiving of multiple PMT messages"""
        msgs = [pmt.to_pmt('test_valid_pmt msg0'),
                pmt.to_pmt('test_valid_pmt msg1'),
                pmt.to_pmt('test_valid_pmt msg2')]
        self.send_messages(msgs)

        time.sleep(0.1)
        self.assertEqual(len(msgs), self.message_debug.num_messages())
        for index, msg in enumerate(msgs):
            self.assertTrue(
                pmt.equal(
                    msg,
                    self.message_debug.get_message(index)))

    def test_invalid_pmt(self):
        """Test receiving of invalid PMT messages"""
        self.recv_request()

        # Do not use pmt.serialize_str here as we don't want a valid PMT
        # message
        self.zmq_sock.send_string('test_invalid_pmt')

        time.sleep(0.1)
        self.assertEqual(0, self.message_debug.num_messages())


if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_req_msg_source)
