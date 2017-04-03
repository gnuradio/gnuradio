#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest, blocks, zeromq
import pmt
import time

class qa_zeromq_msg_pushpull (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001 (self):
        # test msg push pull blocks
        vlen = 10
        src_data = range(vlen)*100
        msgs = pmt.init_f32vector(len(src_data), src_data)
        # The best msg source we have is the strobe, we don't really
        # care how many msgs go through, so just strobe fast enough to
        # guarantee > 1
        msg_src = blocks.message_strobe(msgs, 1)
        msg_sink = blocks.message_debug()
        endpoint = "tcp://127.0.0.1:"
        zeromq_push_sink = zeromq.push_msg_sink(endpoint + "*")
        port = zeromq_push_sink.endpoint().split(":")[-1]
        zeromq_pull_source = zeromq.pull_msg_source(endpoint + port, 0)
        zeromq_pull_source.set_endpoint(endpoint + port)
        self.tb.msg_connect(msg_src, "strobe", zeromq_push_sink, "in")
        self.tb.msg_connect(zeromq_pull_source, "out", msg_sink, "store")
        self.tb.start()
        time.sleep(0.25)
        self.tb.stop()
        self.tb.wait()
        rcvd_message = pmt.f32vector_elements(msg_sink.get_message(0))
        self.assertFloatTuplesAlmostEqual(rcvd_message, src_data)

if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_msg_pushpull)