#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#
"""Unit tests for ZMQ PULL Message Source block"""

import time
import zmq

from gnuradio import gr, gr_unittest, blocks, zeromq
import pmt


class qa_zeromq_pull_msg_source(gr_unittest.TestCase):
    """Unit tests for ZMQ PULL Message Source block"""

    def setUp(self):
        addr = 'tcp://127.0.0.1'
        self.context = zmq.Context()
        self.zmq_sock = self.context.socket(zmq.PUSH)
        port = self.zmq_sock.bind_to_random_port(addr)

        self.zeromq_pull_msg_source = zeromq.pull_msg_source(
            ('%s:%s' % (addr, port)), 100)
        self.message_debug = blocks.message_debug()
        self.tb = gr.top_block()
        self.tb.msg_connect(
            (self.zeromq_pull_msg_source, 'out'), (self.message_debug, 'store'))

        self.tb.start()
        time.sleep(0.1)

    def tearDown(self):
        self.tb.stop()
        self.tb.wait()
        self.zeromq_pull_msg_source = None
        self.message_debug = None
        self.tb = None
        self.zmq_sock.close()
        self.context.term()

    def test_valid_pmt(self):
        """Test receiving of valid PMT messages"""
        msg = pmt.to_pmt('test_valid_pmt')
        self.zmq_sock.send(pmt.serialize_str(msg))
        for _ in range(10):
            if self.message_debug.num_messages() > 0:
                break
            time.sleep(0.2)
        self.assertEqual(1, self.message_debug.num_messages())
        self.assertTrue(pmt.equal(msg, self.message_debug.get_message(0)))

    def test_invalid_pmt(self):
        """Test receiving of invalid PMT messages"""
        self.zmq_sock.send_string('test_invalid_pmt')
        time.sleep(0.1)
        self.assertEqual(0, self.message_debug.num_messages())


if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_pull_msg_source)
