#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import time
import pmt
from gnuradio import gr, gr_unittest, blocks, pdu


class test_flowgraph (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):

        self.tb.start()
        self.tb.lock()

        rem = pdu.pdu_remove(pmt.intern('foo'))
        dbg = blocks.message_debug()
        self.tb.msg_connect((rem, 'pdus'), (dbg, 'store'))

        self.tb.unlock()

        msg = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(3, (1, 2, 3)))
        rem.to_basic_block()._post(pmt.intern('pdus'), msg)
        time.sleep(0.2)

        self.tb.stop()

        self.assertEqual(dbg.num_messages(), 1)
        data = pmt.u8vector_elements(pmt.cdr(dbg.get_message(0)))
        self.assertEqual([1, 2, 3], data)


if __name__ == '__main__':
    gr_unittest.run(test_flowgraph)
