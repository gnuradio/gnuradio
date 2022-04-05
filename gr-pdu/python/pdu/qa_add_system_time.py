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
import time


class qa_add_system_time(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

        self.add_sys_time = pdu.add_system_time(pmt.intern('systime'))
        self.debug = blocks.message_debug()

        # make connections
        self.tb.msg_connect((self.add_sys_time, 'pdu'), (self.debug, 'store'))
        self.tb.msg_connect((self.add_sys_time, 'pdu'), (self.debug, 'print'))

    def tearDown(self):
        self.tb = None

    def test_001_basic_io(self):
        self.tb.start()
        # provide two non PDU inputs and one PDU input
        self.add_sys_time.to_basic_block()._post(
            pmt.intern("pdu"), pmt.intern("BAD PDU"))
        self.add_sys_time.to_basic_block()._post(pmt.intern("pdu"),
                                                 pmt.cons(pmt.from_long(4), pmt.PMT_NIL))
        self.add_sys_time.to_basic_block()._post(pmt.intern("pdu"),
                                                 pmt.cons(pmt.make_dict(), pmt.init_f32vector(1, [0.0])))
        self.waitFor(lambda: self.debug.num_messages() >=
                     1, timeout=1.0, poll_interval=0.01)
        self.tb.stop()
        self.tb.wait()

        # make sure we got one message and it has a systime key
        self.assertEqual(1, self.debug.num_messages())
        self.assertTrue(pmt.dict_has_key(
            pmt.car(self.debug.get_message(0)), pmt.intern('systime')))

    def test_002_timing(self):
        self.tb.start()

        self.add_sys_time.to_basic_block()._post(
            pmt.intern("pdu"), pmt.intern("BAD PDU"))
        self.add_sys_time.to_basic_block()._post(pmt.intern("pdu"),
                                                 pmt.cons(pmt.make_dict(), pmt.init_u8vector(1, [0])))
        # wait for one second to provide a time difference between messages
        time.sleep(1.0)
        self.add_sys_time.to_basic_block()._post(pmt.intern("pdu"),
                                                 pmt.cons(pmt.make_dict(), pmt.init_u8vector(1, [0])))
        self.waitFor(lambda: self.debug.num_messages() ==
                     2, timeout=1.0, poll_interval=0.01)
        self.tb.stop()
        self.tb.wait()

        t0 = pmt.to_double(pmt.dict_ref(pmt.car(self.debug.get_message(0)),
                                        pmt.intern("systime"),
                                        pmt.from_double(0.0)))
        t1 = pmt.to_double(pmt.dict_ref(pmt.car(self.debug.get_message(1)),
                                        pmt.intern("systime"),
                                        pmt.from_double(0.0)))
        # should be sufficient tolerance
        self.assertAlmostEqual(t0, t1 - 1, delta=0.5)


if __name__ == '__main__':
    gr_unittest.run(qa_add_system_time)
