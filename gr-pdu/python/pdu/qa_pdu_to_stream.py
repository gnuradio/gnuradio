#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2018-2021 National Technology & Engineering Solutions of Sandia, LLC
# (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S. Government
# retains certain rights in this software.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, pdu
import numpy as np
import pmt
import time


class qa_pdu_to_bursts (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()
        self.p2s = pdu.pdu_to_stream_c(pdu.EARLY_BURST_APPEND, 64)
        self.vs = blocks.vector_sink_c(1)
        self.tag_debug = blocks.tag_debug(gr.sizeof_gr_complex * 1, '', "")
        self.tag_debug.set_display(True)

        self.tb.connect((self.p2s, 0), (self.vs, 0))
        self.tb.connect((self.p2s, 0), (self.tag_debug, 0))

    def tearDown(self):
        self.tb = None

    def test_001_basic(self):
        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                   1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        in_pdu = pmt.cons(
            pmt.make_dict(), pmt.init_c32vector(len(in_data), in_data))
        e_tag_0 = gr.tag_utils.python_to_tag(
            (0, pmt.intern("tx_sob"), pmt.PMT_T, pmt.PMT_NIL))
        e_tag_1 = gr.tag_utils.python_to_tag(
            (len(in_data) - 1, pmt.intern("tx_eob"), pmt.PMT_T, pmt.PMT_NIL))

        self.tb.start()
        self.p2s.to_basic_block()._post(pmt.intern("pdus"), pmt.intern("MALFORMED PDU"))
        self.p2s.to_basic_block()._post(pmt.intern("pdus"), in_pdu)
        self.waitFor(lambda: len(self.vs.tags()) == 2,
                     timeout=1.0, poll_interval=0.01)
        self.tb.stop()
        self.tb.wait()

        tags = self.vs.tags()
        self.assertEqual(len(tags), 2)
        self.assertEqual(tags[0].offset, e_tag_0.offset)
        self.assertTrue(pmt.equal(tags[0].key, e_tag_0.key))
        self.assertTrue(pmt.equal(tags[0].value, e_tag_0.value))
        self.assertEqual(tags[1].offset, e_tag_1.offset)
        self.assertTrue(pmt.equal(tags[1].key, e_tag_1.key))
        self.assertTrue(pmt.equal(tags[1].value, e_tag_1.value))
        self.assertTrue((in_data == np.real(self.vs.data())).all())

    def test_002_timed(self):
        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                   1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        tag_time = pmt.make_tuple(
            pmt.from_uint64(11), pmt.from_double(0.123456))
        in_dict = pmt.dict_add(
            pmt.make_dict(), pmt.intern("tx_time"), tag_time)
        in_pdu = pmt.cons(in_dict, pmt.init_c32vector(len(in_data), in_data))
        e_tag_0 = gr.tag_utils.python_to_tag(
            (0, pmt.intern("tx_sob"), pmt.PMT_T, pmt.PMT_NIL))
        e_tag_1 = gr.tag_utils.python_to_tag(
            (0, pmt.intern("tx_time"), tag_time, pmt.PMT_NIL))
        e_tag_2 = gr.tag_utils.python_to_tag(
            (len(in_data) - 1, pmt.intern("tx_eob"), pmt.PMT_T, pmt.PMT_NIL))

        self.tb.start()
        self.p2s.to_basic_block()._post(pmt.intern("pdus"), pmt.intern("MALFORMED PDU"))
        self.p2s.to_basic_block()._post(pmt.intern("pdus"), in_pdu)
        self.waitFor(lambda: len(self.vs.tags()) == 3,
                     timeout=1.0, poll_interval=0.01)
        self.tb.stop()
        self.tb.wait()

        tags = self.vs.tags()
        self.assertEqual(len(tags), 3)
        self.assertEqual(tags[0].offset, e_tag_0.offset)
        self.assertTrue(pmt.equal(tags[0].key, e_tag_0.key))
        self.assertTrue(pmt.equal(tags[0].value, e_tag_0.value))
        self.assertEqual(tags[1].offset, e_tag_1.offset)
        self.assertTrue(pmt.equal(tags[1].key, e_tag_1.key))
        self.assertTrue(pmt.equal(tags[1].value, e_tag_1.value))
        self.assertEqual(tags[2].offset, e_tag_2.offset)
        self.assertTrue(pmt.equal(tags[2].key, e_tag_2.key))
        self.assertTrue(pmt.equal(tags[2].value, e_tag_2.value))
        self.assertTrue((in_data == np.real(self.vs.data())).all())

    def test_003_timed_long(self):
        in_data = np.arange(25000).tolist()
        tag_time = pmt.make_tuple(
            pmt.from_uint64(11), pmt.from_double(0.123456))
        in_dict = pmt.dict_add(
            pmt.make_dict(), pmt.intern("tx_time"), tag_time)
        in_pdu = pmt.cons(in_dict, pmt.init_c32vector(len(in_data), in_data))
        e_tag_0 = gr.tag_utils.python_to_tag(
            (0, pmt.intern("tx_sob"), pmt.PMT_T, pmt.PMT_NIL))
        e_tag_1 = gr.tag_utils.python_to_tag(
            (0, pmt.intern("tx_time"), tag_time, pmt.PMT_NIL))
        e_tag_2 = gr.tag_utils.python_to_tag(
            (len(in_data) - 1, pmt.intern("tx_eob"), pmt.PMT_T, pmt.PMT_NIL))

        self.tb.start()
        self.p2s.to_basic_block()._post(pmt.intern("pdus"), pmt.intern("MALFORMED PDU"))
        self.p2s.to_basic_block()._post(pmt.intern("pdus"), in_pdu)
        self.waitFor(lambda: len(self.vs.tags()) == 3,
                     timeout=1.0, poll_interval=0.01)
        self.tb.stop()
        self.tb.wait()

        tags = self.vs.tags()
        self.assertEqual(len(tags), 3)
        self.assertEqual(tags[0].offset, e_tag_0.offset)
        self.assertTrue(pmt.equal(tags[0].key, e_tag_0.key))
        self.assertTrue(pmt.equal(tags[0].value, e_tag_0.value))
        self.assertEqual(tags[1].offset, e_tag_1.offset)
        self.assertTrue(pmt.equal(tags[1].key, e_tag_1.key))
        self.assertTrue(pmt.equal(tags[1].value, e_tag_1.value))
        self.assertEqual(tags[2].offset, e_tag_2.offset)
        self.assertTrue(pmt.equal(tags[2].key, e_tag_2.key))
        self.assertTrue(pmt.equal(tags[2].value, e_tag_2.value))
        self.assertTrue((in_data == np.real(self.vs.data())).all())


if __name__ == '__main__':
    gr_unittest.run(qa_pdu_to_bursts)
