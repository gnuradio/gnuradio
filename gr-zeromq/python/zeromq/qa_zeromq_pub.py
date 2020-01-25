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
from gnuradio import eng_notation
import threading

class qa_zeromq_pub (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        vlen = 10
        self.rx_data = None
        src_data = list(range(vlen))*100
        src = blocks.vector_source_f(src_data, False, vlen)
        zeromq_pub_sink = zeromq.pub_sink(gr.sizeof_float, vlen, "tcp://127.0.0.1:0")
        address = zeromq_pub_sink.last_endpoint()
        self.tb.connect(src, zeromq_pub_sink)
        self.probe_manager = zeromq.probe_manager()
        self.probe_manager.add_socket(address, 'float32', self.recv_data)
        zmq_pull_t = threading.Thread(target=self.probe_manager.watcher)
        zmq_pull_t.daemon = True
        zmq_pull_t.start()
        self.tb.run()
        zmq_pull_t.join(6.0)
        # Check to see if we timed out
        self.assertFalse(zmq_pull_t.is_alive())
        self.assertFloatTuplesAlmostEqual(self.rx_data, src_data)

    def recv_data (self, data):
        self.rx_data = data


if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_pub)
