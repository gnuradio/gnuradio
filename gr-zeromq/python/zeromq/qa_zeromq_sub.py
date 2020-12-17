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

import numpy
import time
import zmq


class qa_zeromq_sub (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.zmq_context = zmq.Context()
        self.pub_socket = self.zmq_context.socket(zmq.PUB)
        self.pub_socket.bind("tcp://127.0.0.1:0")
        self._address = self.pub_socket.getsockopt(zmq.LAST_ENDPOINT).decode()

    def tearDown(self):
        self.pub_socket.close()
        self.zmq_context.term()
        self.tb = None

    def test_001(self):
        vlen = 10
        src_data = numpy.array(list(range(vlen)) * 100, 'float32')
        zeromq_sub_source = zeromq.sub_source(
            gr.sizeof_float, vlen, self._address)
        sink = blocks.vector_sink_f(vlen)
        self.tb.connect(zeromq_sub_source, sink)

        self.tb.start()
        time.sleep(0.05)
        self.pub_socket.send(src_data.tostring())
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()
        self.assertFloatTuplesAlmostEqual(sink.data(), src_data)

    def test_002(self):
        vlen = 10
        # Construct multipart source data to publish
        raw_data = [
            numpy.array(
                range(vlen),
                'float32') *
            100,
            numpy.array(
                range(
                    vlen,
                    2 *
                    vlen),
                'float32') *
            100]
        src_data = [a.tostring() for a in raw_data]
        zeromq_sub_source = zeromq.sub_source(
            gr.sizeof_float, vlen, self._address)
        sink = blocks.vector_sink_f(vlen)
        self.tb.connect(zeromq_sub_source, sink)

        self.tb.start()
        time.sleep(0.05)
        self.pub_socket.send_multipart(src_data)
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()

        # Source block will concatenate everything together
        expected_data = numpy.concatenate(raw_data)
        self.assertFloatTuplesAlmostEqual(sink.data(), expected_data)

    def test_003(self):
        # Check that message is received when correct key is used
        # Construct multipart source data to publish
        vlen = 10
        raw_data = [
            numpy.array(
                range(vlen),
                'float32') *
            100,
            numpy.array(
                range(
                    vlen,
                    2 *
                    vlen),
                'float32') *
            100]
        src_data = [a.tostring() for a in raw_data]

        src_data = [b"filter_key"] + src_data

        zeromq_sub_source = zeromq.sub_source(
            gr.sizeof_float, vlen, self._address, key="filter_key")
        sink = blocks.vector_sink_f(vlen)
        self.tb.connect(zeromq_sub_source, sink)

        self.tb.start()
        time.sleep(0.05)
        self.pub_socket.send_multipart(src_data)
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()

        # Source block will concatenate everything together
        expected_data = numpy.concatenate(raw_data)
        self.assertFloatTuplesAlmostEqual(sink.data(), expected_data)

    def test_004(self):
        # Test that no message is received when wrong key is used
        vlen = 10
        raw_data = [
            numpy.array(
                range(vlen),
                'float32') *
            100,
            numpy.array(
                range(
                    vlen,
                    2 *
                    vlen),
                'float32') *
            100]
        src_data = [a.tostring() for a in raw_data]

        src_data = [b"filter_key"] + src_data

        zeromq_sub_source = zeromq.sub_source(
            gr.sizeof_float, vlen, self._address, key="wrong_filter_key")
        sink = blocks.vector_sink_f(vlen)
        self.tb.connect(zeromq_sub_source, sink)

        self.tb.start()
        time.sleep(0.05)
        self.pub_socket.send_multipart(src_data)
        time.sleep(0.5)
        self.tb.stop()
        self.tb.wait()

        assert(len(sink.data()) == 0)


if __name__ == '__main__':
    gr_unittest.run(qa_zeromq_sub)
