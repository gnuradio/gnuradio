#!/usr/bin/env python
#
# Copyright 2021 NTESS LLC.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, pdu
import pmt
import numpy as np
import time


class qa_time_delta(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

        self.time_delta = pdu.time_delta(pmt.intern(
            "sys time delta (ms)"), pmt.intern("system_time"))
        self.debug = blocks.message_debug()

        self.tb.msg_connect((self.time_delta, 'pdu'), (self.debug, 'store'))

    def tearDown(self):
        self.tb = None

    def test_001_invalid_a(self):
        self.tb.start()
        self.time_delta.to_basic_block()._post(
            pmt.intern("pdu"), pmt.intern("NOT A PDU"))
        time.sleep(0.01)  # short delay to ensure the message is processed
        self.tb.stop()
        self.tb.wait()

        # nothing should be produced in this case
        self.assertEqual(0, self.debug.num_messages())

    def test_001_invalid_b(self):
        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                   1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        meta = pmt.dict_add(pmt.make_dict(), pmt.intern(
            'sam'), pmt.from_double(25.1))
        in_pdu = pmt.cons(meta, pmt.init_c32vector(len(in_data), in_data))

        # set up fg
        self.tb.start()
        self.time_delta.to_basic_block()._post(pmt.intern("pdu"), in_pdu)
        time.sleep(0.01)  # short delay to ensure the message is processed
        self.tb.stop()
        self.tb.wait()

        # nothing should be produced in this case
        self.assertEqual(0, self.debug.num_messages())

    def test_002_normal(self):
        tnow = time.time()

        in_data = [0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                   1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        meta = pmt.dict_add(pmt.make_dict(), pmt.intern(
            'system_time'), pmt.from_double(tnow - 10.0))
        in_pdu = pmt.cons(meta, pmt.init_c32vector(len(in_data), in_data))

        # set up fg
        self.tb.start()
        self.time_delta.to_basic_block()._post(pmt.intern("pdu"), in_pdu)
        self.waitFor(lambda: self.debug.num_messages() ==
                     1, timeout=1.0, poll_interval=0.01)
        self.tb.stop()
        self.tb.wait()

        # check data
        self.assertEqual(1, self.debug.num_messages())
        a_meta = pmt.car(self.debug.get_message(0))
        time_tag = pmt.dict_ref(a_meta, pmt.intern("system_time"), pmt.PMT_NIL)
        delta_tag = pmt.dict_ref(a_meta, pmt.intern(
            "sys time delta (ms)"), pmt.PMT_NIL)
        self.assertAlmostEqual(tnow - 10.0, pmt.to_double(time_tag), delta=1e-6)
        self.assertAlmostEqual(10000, pmt.to_double(delta_tag), delta=500)


if __name__ == '__main__':
    gr_unittest.run(qa_time_delta)
