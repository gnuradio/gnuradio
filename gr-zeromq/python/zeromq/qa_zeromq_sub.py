#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2014 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
from gnuradio import blocks, zeromq
from gnuradio import eng_notation

import numpy
import time
import zmq

class qa_zeromq_sub (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()
        self.zmq_context = zmq.Context()
        self.pub_socket = self.zmq_context.socket(zmq.PUB)
        self.pub_socket.bind("tcp://127.0.0.1:5559")

    def tearDown (self):
        self.pub_socket.close()
        self.zmq_context.term()
        self.tb = None

    def test_001 (self):
        vlen = 10
        src_data = numpy.array(range(vlen)*100, 'float32')
        zeromq_sub_source = zeromq.sub_source(gr.sizeof_float, vlen, "tcp://127.0.0.1:5559")
        sink = blocks.vector_sink_f(vlen)
        self.tb.connect(zeromq_sub_source, sink)

        self.tb.start()
        time.sleep(0.05)
        self.pub_socket.send(src_data.tostring())
        time.sleep(0.25)
        self.tb.stop()
        self.tb.wait()
        self.assertFloatTuplesAlmostEqual(sink.data(), src_data)

    def test_002 (self):
        vlen = 10

        # Construct multipart source data to publish
        raw_data = [numpy.array(range(vlen)*100, 'float32'), numpy.array(range(vlen, 2*vlen)*100, 'float32')]
        src_data = [a.tostring() for a in raw_data]
        zeromq_sub_source = zeromq.sub_source(gr.sizeof_float, vlen, "tcp://127.0.0.1:5559")
        sink = blocks.vector_sink_f(vlen)
        self.tb.connect(zeromq_sub_source, sink)

        self.tb.start()
        time.sleep(0.05)
        self.pub_socket.send_multipart(src_data)
        time.sleep(0.25)
        self.tb.stop()
        self.tb.wait()

        # Source block will concatenate everything together
        expected_data = numpy.concatenate(raw_data)
        self.assertFloatTuplesAlmostEqual(sink.data(), expected_data)


if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_sub)
