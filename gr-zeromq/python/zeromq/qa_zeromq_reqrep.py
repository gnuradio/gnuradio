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
import time

class qa_zeromq_reqrep (gr_unittest.TestCase):

    def setUp (self):
        self.send_tb = gr.top_block()
        self.recv_tb = gr.top_block()

    def tearDown (self):
        self.send_tb = None
        self.recv_tb = None

    def test_001 (self):
        vlen = 10
        src_data = list(range(vlen))*100
        src = blocks.vector_source_f(src_data, False, vlen)
        zeromq_rep_sink = zeromq.rep_sink(gr.sizeof_float, vlen, "tcp://127.0.0.1:0", 0)
        address = zeromq_rep_sink.last_endpoint()
        zeromq_req_source = zeromq.req_source(gr.sizeof_float, vlen, address, 0)
        sink = blocks.vector_sink_f(vlen)
        self.send_tb.connect(src, zeromq_rep_sink)
        self.recv_tb.connect(zeromq_req_source, sink)
        self.recv_tb.start()
        time.sleep(0.5)
        self.send_tb.start()
        time.sleep(0.5)
        self.recv_tb.stop()
        self.send_tb.stop()
        self.recv_tb.wait()
        self.send_tb.wait()
        self.assertFloatTuplesAlmostEqual(sink.data(), src_data)

if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_reqrep)
