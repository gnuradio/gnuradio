#!/usr/bin/env python
#
# Copyright 2015,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import time
import struct

import pmt
from gnuradio import gr, gr_unittest, digital, blocks
from gnuradio.digital import packet_utils


class test_packet_format_fb(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_packet_format_async_default(self):
        ac = packet_utils.default_access_code
        hdr_format = digital.header_format_default(ac, 0)

        formatter = digital.protocol_formatter_async(hdr_format)
        snk_hdr = blocks.message_debug()
        snk_pld = blocks.message_debug()

        self.tb.msg_connect(formatter, 'header', snk_hdr, 'store')
        self.tb.msg_connect(formatter, 'payload', snk_pld, 'store')

        send_str = b"Hello World"
        send_pmt = pmt.make_u8vector(len(send_str), 0)
        for i in range(len(send_str)):
            pmt.u8vector_set(send_pmt, i, send_str[i])
        msg = pmt.cons(pmt.PMT_NIL, send_pmt)

        port = pmt.intern("in")
        formatter.to_basic_block()._post(port, msg)

        self.tb.start()
        while (snk_hdr.num_messages() < 1) or (snk_pld.num_messages() < 1):
            time.sleep(0.1)

        self.tb.stop()
        self.tb.wait()

        result_hdr_pmt = pmt.cdr(snk_hdr.get_message(0))
        result_pld_pmt = pmt.cdr(snk_pld.get_message(0))

        result_hdr = pmt.u8vector_elements(result_hdr_pmt)
        result_pld = pmt.u8vector_elements(result_pld_pmt)
        header = bytes(result_hdr)
        payload = bytes(result_pld)

        access_code = packet_utils.default_access_code_binary
        rx_access_code = header[0:len(access_code)]

        length = len(send_str)
        rx_length = struct.unpack_from(b"!H", header, len(access_code))[0]

        self.assertEqual(access_code, rx_access_code)
        self.assertEqual(length, rx_length)
        self.assertEqual(length, len(payload))
        self.assertEqual(send_str, payload[0:length])

    def test_packet_parse_default(self):
        ac = packet_utils.default_access_code
        length = '0000000000000001'

        hdr_format_1bps = digital.header_format_default(ac, 0)
        hdr_format_4bps = digital.header_format_default(ac, 0, 4)

        ac_bits = [int(x) & 1 for x in ac]
        length_bits = [int(x) & 1 for x in length]
        header_bits = ac_bits + length_bits + length_bits

        src_hdr = blocks.vector_source_b(header_bits)

        parser_1bps = digital.protocol_parser_b(hdr_format_1bps)
        parser_4bps = digital.protocol_parser_b(hdr_format_4bps)

        snk_hdr_1bps = blocks.message_debug()
        snk_hdr_4bps = blocks.message_debug()

        self.tb.connect(src_hdr, parser_1bps)
        self.tb.connect(src_hdr, parser_4bps)

        self.tb.msg_connect(parser_1bps, 'info', snk_hdr_1bps, 'store')
        self.tb.msg_connect(parser_4bps, 'info', snk_hdr_4bps, 'store')

        self.tb.start()
        while (
                snk_hdr_1bps.num_messages() < 1) or (
                snk_hdr_4bps.num_messages() < 1):
            time.sleep(0.1)
        self.tb.stop()
        self.tb.wait()

        result_1bps = snk_hdr_1bps.get_message(0)
        result_4bps = snk_hdr_4bps.get_message(0)

        self.assertTrue(pmt.dict_has_key(
            result_1bps, pmt.intern('payload symbols')))
        self.assertEqual(pmt.to_long(pmt.dict_ref(
            result_1bps, pmt.intern('payload symbols'), pmt.PMT_F)), 8)

        self.assertTrue(pmt.dict_has_key(
            result_4bps, pmt.intern('payload symbols')))
        self.assertEqual(pmt.to_long(pmt.dict_ref(
            result_4bps, pmt.intern('payload symbols'), pmt.PMT_F)), 2)

    def test_packet_format_async_counter(self):
        bps = 2
        ac = packet_utils.default_access_code
        hdr_format = digital.header_format_counter(ac, 0, 2)

        formatter = digital.protocol_formatter_async(hdr_format)
        snk_hdr = blocks.message_debug()
        snk_pld = blocks.message_debug()

        self.tb.msg_connect(formatter, 'header', snk_hdr, 'store')
        self.tb.msg_connect(formatter, 'payload', snk_pld, 'store')

        send_str = b"Hello World" + 1000 * b"xxx"
        send_pmt = pmt.make_u8vector(len(send_str), 0)
        for i in range(len(send_str)):
            pmt.u8vector_set(send_pmt, i, send_str[i])
        msg = pmt.cons(pmt.PMT_NIL, send_pmt)

        port = pmt.intern("in")
        formatter.to_basic_block()._post(port, msg)

        self.tb.start()
        while (snk_hdr.num_messages() < 1) or (snk_pld.num_messages() < 1):
            time.sleep(0.1)
        self.tb.stop()
        self.tb.wait()

        result_hdr_pmt = pmt.cdr(snk_hdr.get_message(0))
        result_pld_pmt = pmt.cdr(snk_pld.get_message(0))

        result_hdr = pmt.u8vector_elements(result_hdr_pmt)
        result_pld = pmt.u8vector_elements(result_pld_pmt)
        header = bytes(result_hdr)
        payload = bytes(result_pld)

        access_code = packet_utils.default_access_code_binary
        rx_access_code = header[0:len(access_code)]

        length = len(send_str)
        rx_length = struct.unpack_from(b"!H", header, len(access_code))[0]
        rx_bps = struct.unpack_from(b"!H", header, len(access_code) + 4)[0]
        rx_counter = struct.unpack_from(b"!H", header, len(access_code) + 6)[0]

        self.assertEqual(access_code, rx_access_code)
        self.assertEqual(length, rx_length)
        self.assertEqual(bps, rx_bps)
        self.assertEqual(0, rx_counter)
        self.assertEqual(length, len(payload))
        self.assertEqual(send_str, payload[0:length])

    def test_packet_format_ofdm(self):
        """
        Test the header_format_ofdm object with scrambling
        """
        bphs = 8  # Header bps
        bpps = 8  # Payload bps
        scrambling = True
        occupied_carriers = [list(range(0, 40))]
        len_key_name = "packet_len"
        frame_key_name = "frame_len"
        num_key_name = "packet_num"
        hdr_format = digital.header_format_ofdm(
            occupied_carriers,
            1,
            len_key_name,
            frame_key_name,
            num_key_name,
            bphs,
            bpps,
            scrambling,
        )

        formatter = digital.protocol_formatter_bb(hdr_format, len_key_name)
        parser = digital.protocol_parser_b(hdr_format)

        send_str = b"Hello World" + 100 * b"xxx"
        send_bits = list(send_str)
        src = blocks.vector_source_b(
            send_bits + send_bits,
            repeat=False,
            tags=[
                gr.python_to_tag(
                    [
                        0,
                        pmt.intern(len_key_name),
                        pmt.to_pmt(len(send_bits)),
                        pmt.intern("vector_source"),
                    ]
                ),
                gr.python_to_tag(
                    [
                        len(send_bits),
                        pmt.intern(len_key_name),
                        pmt.to_pmt(len(send_bits)),
                        pmt.intern("vector_source"),
                    ]
                ),
            ],
        )

        repack = blocks.repack_bits_bb(8, 1, len_key_name, False, gr.GR_LSB_FIRST)
        snk_hdr = blocks.message_debug()

        self.tb.connect(src, formatter)
        self.tb.connect(formatter, repack)
        self.tb.connect(repack, parser)

        self.tb.msg_connect(parser, "info", snk_hdr, "store")
        self.tb.msg_connect(parser, "info", snk_hdr, "print")

        self.tb.start()
        while snk_hdr.num_messages() < 2:
            time.sleep(0.1)
        self.tb.stop()
        self.tb.wait()

        result_hdr_pmt = snk_hdr.get_message(0)

        self.assertTrue(pmt.dict_has_key(result_hdr_pmt, pmt.intern(frame_key_name)))
        self.assertEqual(
            pmt.to_long(
                pmt.dict_ref(result_hdr_pmt, pmt.intern(frame_key_name), pmt.PMT_F)
            ),
            len(send_str) // len(occupied_carriers[0]) + 1,
        )
        self.assertTrue(pmt.dict_has_key(result_hdr_pmt, pmt.intern(len_key_name)))
        self.assertEqual(
            pmt.to_long(
                pmt.dict_ref(result_hdr_pmt, pmt.intern(len_key_name), pmt.PMT_F)
            ),
            len(send_str),
        )
        self.assertTrue(pmt.dict_has_key(result_hdr_pmt, pmt.intern(num_key_name)))
        self.assertEqual(
            pmt.to_long(
                pmt.dict_ref(result_hdr_pmt, pmt.intern(num_key_name), pmt.PMT_F)
            ),
            0,
        )

        result_hdr_pmt = snk_hdr.get_message(1)

        self.assertTrue(pmt.dict_has_key(result_hdr_pmt, pmt.intern(frame_key_name)))
        self.assertEqual(
            pmt.to_long(
                pmt.dict_ref(result_hdr_pmt, pmt.intern(frame_key_name), pmt.PMT_F)
            ),
            len(send_str) // len(occupied_carriers[0]) + 1,
        )
        self.assertTrue(pmt.dict_has_key(result_hdr_pmt, pmt.intern(len_key_name)))
        self.assertEqual(
            pmt.to_long(
                pmt.dict_ref(result_hdr_pmt, pmt.intern(len_key_name), pmt.PMT_F)
            ),
            len(send_str),
        )
        self.assertTrue(pmt.dict_has_key(result_hdr_pmt, pmt.intern(num_key_name)))
        self.assertEqual(
            pmt.to_long(
                pmt.dict_ref(result_hdr_pmt, pmt.intern(num_key_name), pmt.PMT_F)
            ),
            1,
        )


if __name__ == '__main__':
    gr_unittest.run(test_packet_format_fb)
