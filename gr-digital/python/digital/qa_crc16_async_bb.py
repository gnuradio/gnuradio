#!/usr/bin/env python
# Copyright 2022 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, digital
import pmt


class qa_crc16_async_bb(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()
        self.tsb_key = "length"

    def tearDown(self):
        self.tb = None

    def test_crc16(self):
        crc_append_block = digital.crc16_async_bb(check=False)
        crc_check_block = digital.crc16_async_bb(check=True)

        dbg_append = blocks.message_debug()
        dbg_check = blocks.message_debug()

        self.tb.msg_connect((crc_append_block, 'out'), (crc_check_block, 'in'))
        self.tb.msg_connect((crc_append_block, 'out'), (dbg_append, 'store'))
        self.tb.msg_connect((crc_check_block, 'out'), (dbg_check, 'store'))

        data = list(range(16))
        pdu = pmt.cons(pmt.PMT_NIL, pmt.init_u8vector(len(data), data))

        crc_append_block._post(pmt.intern('in'), pdu)
        crc_append_block._post(
            pmt.intern('system'),
            pmt.cons(pmt.intern('done'), pmt.from_long(1)))

        self.tb.run()

        self.assertEqual(dbg_append.num_messages(), 1)
        out_append = pmt.u8vector_elements(pmt.cdr(dbg_append.get_message(0)))
        self.assertEqual(out_append, data + [0x3b, 0x37])

        self.assertEqual(dbg_check.num_messages(), 1)
        out_check = pmt.u8vector_elements(pmt.cdr(dbg_check.get_message(0)))
        self.assertEqual(out_check, data)


if __name__ == '__main__':
    gr_unittest.run(qa_crc16_async_bb)
