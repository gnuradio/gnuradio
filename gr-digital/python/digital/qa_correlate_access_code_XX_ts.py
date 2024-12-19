#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2011,2013,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks
import pmt
import numpy
import struct

default_access_code = '\xAC\xDD\xA4\xE2\xF2\x8C\x20\xFC'


def string_to_1_0_list(s):
    r = []
    for ch in s:
        x = ord(ch)
        for i in range(8):
            t = (x >> i) & 0x1
            r.append(t)
    return r


def to_1_0_string(L):
    return ''.join([chr(x + ord('0')) for x in L])


class test_correlate_access_code_XX_ts(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        payload = "test packet"     # payload length is 11 bytes
        # header contains packet length, twice (bit-swapped)
        header = "\x00\xd0\x00\xd0"
        packet = header + payload
        pad = (0,) * 64
        src_data = (0, 0, 1, 1, 1, 1, 0, 1, 1) + \
            tuple(string_to_1_0_list(packet)) + pad
        expected = list(map(int, src_data[9 + 32:-len(pad)]))
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_bb_ts("1011", 0, "sync")
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(len(result_data), len(payload) * 8)
        self.assertEqual(result_tags[0].offset, 0)
        self.assertEqual(pmt.to_long(result_tags[0].value), len(payload) * 8)
        self.assertEqual(result_data, expected)

    def _variable_payload_t(self, payload_len=2**15 + 3):
        # Constants for this test
        SEED = 42
        PRECEDING_NOISE_LEN = 2**10 + 1
        PREAMBLE_LEN = 31
        PADDING_LEN = 64

        numpy.random.seed(SEED)
        noise_pre = numpy.random.randint(0, 2, size=PRECEDING_NOISE_LEN, dtype=numpy.uint8)
        access_code = numpy.random.randint(0, 2, size=PREAMBLE_LEN, dtype=numpy.uint8)
        payload = numpy.random.randint(0, 256, size=payload_len, dtype=numpy.uint8)

        # header contains packet length, twice (bit-swapped)
        header = numpy.array([(payload_len & 0xFF00) >> 8, payload_len & 0xFF] * 2, dtype=numpy.uint8)
        # make sure we've built the length header correctly
        length1, length2 = struct.unpack(">HH", header)
        self.assertEqual(length1, length2)
        self.assertEqual(length1, len(payload))

        packet = numpy.concatenate((header, payload))
        pad = (0,) * PADDING_LEN
        src_data = numpy.concatenate((
            noise_pre,
            access_code,
            numpy.unpackbits(packet),
            numpy.array(pad, dtype=numpy.uint8)
        ))

        expected = list(src_data[len(noise_pre) + len(access_code) + 32:-len(pad)])
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_bb_ts("".join((str(b) for b in access_code)), 0, "sync")
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(len(result_data), len(payload) * 8)
        self.assertEqual(result_tags[0].offset, 0)
        self.assertEqual(pmt.to_long(result_tags[0].value), len(payload) * 8)
        self.assertEqual(result_data, expected)

    def test_payload_lengths(self):
        for length in (2**15 + 3, 1, 10, 100, 300, 301, 400, 2**12, 2**16 - 1):
            self._variable_payload_t(length)

    def test_bb_prefix(self):
        payload = "test packet"     # payload length is 11 bytes
        # header contains packet length, twice (bit-swapped)
        header = "\x00\xd0\x00\xd0"
        packet = header + payload
        pad = (0,) * 64
        src_data = (0, 1, 1, 1, 0, 0, 0, 1, 1) + \
            tuple(string_to_1_0_list(packet)) + pad
        expected = list(map(int, src_data[9 + 32:-len(pad)]))
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_bb_ts("0011", 0, "sync")
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(len(result_data), len(payload) * 8)
        self.assertEqual(result_tags[0].offset, 0)
        self.assertEqual(pmt.to_long(result_tags[0].value), len(payload) * 8)
        self.assertEqual(result_data, expected)

    def test_bb_immediate(self):
        """Test that packets at start of stream match"""
        payload = "test packet"     # payload length is 11 bytes
        # header contains packet length, twice (bit-swapped)
        header = "\x00\xd0\x00\xd0"
        packet = header + payload
        pad = (0,) * 64
        src_data = (0, 0, 1, 1) + tuple(string_to_1_0_list(packet)) + pad
        expected = list(map(int, src_data[4 + 32:-len(pad)]))
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_bb_ts("0011", 0, "sync")
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        # self.assertEqual(len(result_data), len(packet)*8)
        self.assertEqual(result_tags[0].offset, 0)
        # self.assertEqual(pmt.to_long(result_tags[0].value), len(payload)*8)
        self.assertEqual(result_data, expected)

    def test_002(self):
        payload = "test packet"     # payload length is 11 bytes
        # header contains packet length, twice (bit-swapped)
        header = "\x00\xd0\x00\xd0"
        packet = header + payload
        pad = (0,) * 64
        src_data = (0, 0, 1, 1, 1, 1, 0, 1, 1) + \
            tuple(string_to_1_0_list(packet)) + pad
        # convert to binary antipodal symbols (-1,1)
        src_floats = tuple(2 * b - 1 for b in src_data)
        expected = src_floats[9 + 32:-len(pad)]
        src = blocks.vector_source_f(src_floats)
        op = digital.correlate_access_code_ff_ts("1011", 0, "sync")
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(len(result_data), len(payload) * 8)
        self.assertEqual(result_tags[0].offset, 0)
        self.assertEqual(pmt.to_long(result_tags[0].value), len(payload) * 8)
        self.assertFloatTuplesAlmostEqual(result_data, expected, 5)

    def test_ff_prefix(self):
        payload = "test packet"     # payload length is 11 bytes
        # header contains packet length, twice (bit-swapped)
        header = "\x00\xd0\x00\xd0"
        packet = header + payload
        pad = (0,) * 64
        src_data = (0, 1, 1, 1, 1, 0, 0, 1, 1) + \
            tuple(string_to_1_0_list(packet)) + pad
        # convert to binary antipodal symbols (-1,1)
        src_floats = tuple(2 * b - 1 for b in src_data)
        expected = src_floats[9 + 32:-len(pad)]
        src = blocks.vector_source_f(src_floats)
        op = digital.correlate_access_code_ff_ts("0011", 0, "sync")
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(len(result_data), len(payload) * 8)
        self.assertEqual(result_tags[0].offset, 0)
        self.assertEqual(pmt.to_long(result_tags[0].value), len(payload) * 8)
        self.assertFloatTuplesAlmostEqual(result_data, expected, 5)

    def test_ff_immediate(self):
        """Test that packets at start of stream match"""
        payload = "test packet"     # payload length is 11 bytes
        # header contains packet length, twice (bit-swapped)
        header = "\x00\xd0\x00\xd0"
        packet = header + payload
        pad = (0,) * 64
        src_data = (0, 0, 1, 1) + tuple(string_to_1_0_list(packet)) + pad
        # convert to binary antipodal symbols (-1,1)
        src_floats = tuple(2 * b - 1 for b in src_data)
        expected = src_floats[4 + 32:-len(pad)]
        src = blocks.vector_source_f(src_floats)
        op = digital.correlate_access_code_ff_ts("0011", 0, "sync")
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(len(result_data), len(payload) * 8)
        self.assertEqual(result_tags[0].offset, 0)
        self.assertEqual(pmt.to_long(result_tags[0].value), len(payload) * 8)
        self.assertFloatTuplesAlmostEqual(result_data, expected, 5)


if __name__ == '__main__':
    gr_unittest.run(test_correlate_access_code_XX_ts)
