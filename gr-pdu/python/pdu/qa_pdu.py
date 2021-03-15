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
import pmt
import time


class test_pdu(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        # Just run some data through and make sure it doesn't puke.
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)

        src = pdu.pdu_to_tagged_stream(gr.types.byte_t)
        snk3 = pdu.tagged_stream_to_pdu(gr.types.byte_t)
        snk2 = blocks.vector_sink_b()
        snk = blocks.tag_debug(1, "test")
        snk.set_display(False)

        dbg = blocks.message_debug()

        # Test that the right number of ports exist.
        pi = snk3.message_ports_in()
        po = snk3.message_ports_out()
        # system port is defined automatically
        self.assertEqual(pmt.length(pi), 1)
        self.assertEqual(pmt.length(po), 1)

        self.tb.connect(src, snk)
        self.tb.connect(src, snk2)
        self.tb.connect(src, snk3)
        self.tb.msg_connect(snk3, "pdus", dbg, "store")

        # make our reference and message pmts
        port = pmt.intern("pdus")
        msg = pmt.cons(pmt.PMT_NIL, pmt.make_u8vector(16, 0xFF))

        # post the message
        src.to_basic_block()._post(port, msg)
        src.to_basic_block()._post(
            pmt.intern("system"), pmt.cons(
                pmt.intern("done"), pmt.from_long(1)))

        self.tb.start()
        self.tb.wait()

        # Get the vector of data from the vector sink
        result_data = snk2.data()

        # Get the vector of data from the message sink
        # Convert the message PMT as a pair into its vector
        result_msg = dbg.get_message(0)
        msg_vec = pmt.cdr(result_msg)
        # pmt.print(msg_vec)

        # Convert the PMT vector into a Python list
        msg_data = []
        for i in range(16):
            msg_data.append(pmt.u8vector_ref(msg_vec, i))

        actual_data = 16 * [0xFF, ]
        self.assertEqual(actual_data, list(result_data))
        self.assertEqual(actual_data, msg_data)

    def test_001(self):
        # Test the overflow buffer in pdu_to_tagged_stream
        src_data = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]
        src = pdu.pdu_to_tagged_stream(gr.types.float_t)
        snk = blocks.vector_sink_f()

        self.tb.connect(src, snk)
        port = pmt.intern("pdus")

        msg = pmt.cons(pmt.PMT_NIL, pmt.init_f32vector(10, src_data))
        src.to_basic_block()._post(port, msg)
        src.to_basic_block()._post(
            pmt.intern("system"), pmt.cons(
                pmt.intern("done"), pmt.from_long(1)))

        self.tb.start()
        self.tb.wait()

        self.assertEqual(src_data, list(snk.data()))

    def test_002_tags_plus_data(self):
        packet_len = 16
        src_data = list(range(packet_len))
        tag1 = gr.tag_t()
        tag1.offset = 0
        tag1.key = pmt.string_to_symbol('spam')
        tag1.value = pmt.from_long(23)
        tag2 = gr.tag_t()
        tag2.offset = 10  # Must be < packet_len
        tag2.key = pmt.string_to_symbol('eggs')
        tag2.value = pmt.from_long(42)
        src = blocks.vector_source_f(src_data, tags=(tag1, tag2))
        s2ts = blocks.stream_to_tagged_stream(
            gr.sizeof_float,
            vlen=1,
            packet_len=packet_len,
            len_tag_key="packet_len")
        ts2pdu = pdu.tagged_stream_to_pdu(gr.types.float_t, "packet_len")
        dbg = blocks.message_debug()
        self.tb.connect(src, s2ts, ts2pdu)
        self.tb.msg_connect(ts2pdu, "pdus", dbg, "store")
        self.tb.start()
        self.tb.wait()
        result_msg = dbg.get_message(0)
        metadata = pmt.to_python(pmt.car(result_msg))
        vector = pmt.f32vector_elements(pmt.cdr(result_msg))
        self.assertEqual(metadata, {'eggs': 42, 'spam': 23})
        self.assertFloatTuplesAlmostEqual(tuple(vector), src_data)


if __name__ == '__main__':
    gr_unittest.run(test_pdu)
