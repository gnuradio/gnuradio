#!/usr/bin/env python3
# Copyright 2022 Daniel Estevez <daniel@destevez.net>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#


from gnuradio import gr, gr_unittest, blocks, digital
import pmt


class qa_crc(gr_unittest.TestCase):
    def setUp(self):
        """Common part of all CRC tests

        Creates a flowgraph, a Message Debug block, and a PDU
        containing the numbers 0x00 through 0x0F.
        """
        self.tb = gr.top_block()
        self.dbg = blocks.message_debug()
        self.data = list(range(16))
        self.pdu = pmt.cons(pmt.PMT_NIL,
                            pmt.init_u8vector(len(self.data), self.data))

    def run_crc_append(self, crc_params, crc_result):
        """Common part of CRC Append tests

        Creates a CRC Append block with the specified crc_params parameters,
        connects it to the Message Debug block, sends a test PDU to the
        CRC Append block, and checks that the output PDU matches the expected
        crc_result.
        """
        crc_append_block = digital.crc_append(*crc_params)
        self.tb.msg_connect((crc_append_block, 'out'), (self.dbg, 'store'))
        crc_append_block.to_basic_block()._post(pmt.intern('in'), self.pdu)
        crc_append_block.to_basic_block()._post(
            pmt.intern('system'),
            pmt.cons(pmt.intern('done'), pmt.from_long(1)))

        self.tb.start()
        self.tb.wait()

        self.assertEqual(self.dbg.num_messages(), 1)
        out = pmt.u8vector_elements(pmt.cdr(self.dbg.get_message(0)))
        self.assertEqual(out[:len(self.data)], self.data)
        self.assertEqual(out[len(self.data):], crc_result)

    def common_test_crc_check(self, matching_crc, header_bytes=0):
        """Common part of CRC Check tests

        Creates a CRC Append block and a CRC Check block using either the
        same CRC or a different one depending on the whether matching_crc
        is True or False. Connects CRC Append -> CRC Check -> Message Debug
        and sends a PDU through. There are two message debugs to allow
        checking whether the PDU ended up in the ok or fail port of the
        CRC Check block.
        """
        crc_append_block = digital.crc_append(
            16, 0x1021, 0x0, 0x0, False, False, False, header_bytes)
        x = 0x0 if matching_crc else 0xFFFF
        crc_check_block = digital.crc_check(
            16, 0x1021, x, x, False, False, False, True, header_bytes)

        self.dbg_fail = blocks.message_debug()
        self.tb.msg_connect((crc_append_block, 'out'), (crc_check_block, 'in'))
        self.tb.msg_connect((crc_check_block, 'ok'), (self.dbg, 'store'))
        self.tb.msg_connect((crc_check_block, 'fail'),
                            (self.dbg_fail, 'store'))

        crc_append_block.to_basic_block()._post(pmt.intern('in'), self.pdu)
        crc_append_block.to_basic_block()._post(
            pmt.intern('system'),
            pmt.cons(pmt.intern('done'), pmt.from_long(1)))
        self.tb.start()
        self.tb.wait()

    def test_crc_check(self):
        """Test a successful CRC check

        Checks that the PDU ends in the ok port of CRC check
        """
        self.common_test_crc_check(matching_crc=True)
        self.assertEqual(self.dbg.num_messages(), 1)
        out = pmt.u8vector_elements(pmt.cdr(self.dbg.get_message(0)))
        self.assertEqual(out, self.data)
        self.assertEqual(self.dbg_fail.num_messages(), 0)

    def test_crc_check_header_bytes(self):
        """Test a successful CRC check (skipping some header bytes)

        Checks that the PDU ends in the ok port of CRC check
        """
        self.common_test_crc_check(matching_crc=True, header_bytes=5)
        self.assertEqual(self.dbg.num_messages(), 1)
        out = pmt.u8vector_elements(pmt.cdr(self.dbg.get_message(0)))
        self.assertEqual(out, self.data)
        self.assertEqual(self.dbg_fail.num_messages(), 0)

    def test_crc_check_wrong_crc(self):
        """Test a failed CRC check

        Checks that the PDU ends in the fail port of CRC check
        """
        self.common_test_crc_check(matching_crc=False)
        self.assertEqual(self.dbg.num_messages(), 0)
        self.assertEqual(self.dbg_fail.num_messages(), 1)
        out = pmt.u8vector_elements(pmt.cdr(self.dbg_fail.get_message(0)))
        self.assertEqual(out, self.data)

    def test_crc_append_crc16_ccitt_zero(self):
        """Test CRC-16-CCITT-Zero calculation"""
        self.run_crc_append(
            (16, 0x1021, 0x0, 0x0,
             False, False, False),
            [0x51, 0x3D])

    def test_crc_append_crc16_ccitt_false(self):
        """Test CRC-16-CCITT-False calculation"""
        self.run_crc_append(
            (16, 0x1021, 0xFFFF, 0x0,
             False, False, False),
            [0x3B, 0x37])

    def test_crc_append_crc16_ccitt_x25(self):
        """Test CRC-16-CCITT-X.25 calculation"""
        self.run_crc_append(
            (16, 0x1021, 0xFFFF, 0xFFFF,
             True, True, False),
            [0x13, 0xE9])

    def test_crc_append_crc32(self):
        """Test CRC-32 calculation"""
        self.run_crc_append(
            (32, 0x4C11DB7, 0xFFFFFFFF, 0xFFFFFFFF,
             True, True, False),
            [0xCE, 0xCE, 0xE2, 0x88])

    def test_crc_append_crc32c(self):
        """Test CRC-32C calculation"""
        self.run_crc_append(
            (32, 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF,
             True, True, False),
            [0xD9, 0xC9, 0x08, 0xEB])

    def test_crc_append_crc32c_endianness_swap(self):
        """Test CRC-32C calculation with endianness swapped"""
        self.run_crc_append(
            (32, 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF,
             True, True, True),
            [0xEB, 0x08, 0xC9, 0xD9])

    def test_crc_append_crc32c_skip_header_bytes(self):
        """Test CRC-32C calculation skipping some header bytes"""
        skip_bytes = 3
        self.run_crc_append(
            (32, 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF,
             True, True, False, skip_bytes),
            [0xE8, 0x62, 0x60, 0x68])


class qa_crc_class(gr_unittest.TestCase):
    def test_crc_crc32c(self):
        """Test CRC-32C calculation (using crc class directly)"""
        c = digital.crc(32, 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF, True, True)
        out = c.compute(list(range(16)))
        self.assertEqual(c.compute(list(range(16))),
                         0xD9C908EB)


if __name__ == '__main__':
    gr_unittest.run(qa_crc)
    gr_unittest.run(qa_crc_class)
