#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy as np

from gnuradio import gr, gr_unittest, blocks, fec
from gnuradio.fec.extended_encoder import extended_encoder
from gnuradio.fec.polar.encoder import PolarEncoder
from gnuradio.fec.polar import channel_construction as cc

# import os
# print('PID:', os.getpid())
# raw_input('tell me smth')


class test_polar_encoder(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        block_size = 16
        num_info_bits = 8
        frozen_bit_positions = cc.frozen_bit_positions(
            block_size, num_info_bits, 0.0)
        frozen_bit_values = np.array([],)

        polar_encoder = fec.polar_encoder.make(
            block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        self.assertEqual(block_size, polar_encoder.get_output_size())
        self.assertEqual(num_info_bits, polar_encoder.get_input_size())
        self.assertFloatTuplesAlmostEqual(
            (float(num_info_bits) / block_size, ), (polar_encoder.rate(), ))
        self.assertFalse(polar_encoder.set_frame_size(10))

    def test_002_work_function_packed(self):
        is_packed = True
        block_size = 256
        num_info_bits = block_size // 2

        data, ref, polar_encoder = self.get_test_data(
            block_size, num_info_bits, 1, is_packed)
        src = blocks.vector_source_b(data, False)
        enc_block = extended_encoder(polar_encoder, None, '11')
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_003_work_function_unpacked(self):
        is_packed = False
        block_size = 256
        num_info_bits = block_size // 2

        data, ref, polar_encoder = self.get_test_data(
            block_size, num_info_bits, 1, is_packed)
        src = blocks.vector_source_b(data, False)
        enc_block = extended_encoder(polar_encoder, None, '11')
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_004_big_input(self):
        is_packed = False
        num_blocks = 30
        block_size = 1024
        num_info_bits = block_size // 8

        data, ref, polar_encoder = self.get_test_data(
            block_size, num_info_bits, num_blocks, is_packed)
        src = blocks.vector_source_b(data, False)
        enc_block = extended_encoder(polar_encoder, None, '11')
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(ref))

    def get_test_data(self, block_size, num_info_bits, num_blocks, is_packed):
        # helper function to set up test data and together with encoder object.
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = cc.frozen_bit_positions(
            block_size, num_info_bits, 0.0)
        frozen_bit_values = np.array([0] * num_frozen_bits,)
        python_encoder = PolarEncoder(
            block_size,
            num_info_bits,
            frozen_bit_positions,
            frozen_bit_values)

        data = np.array([], dtype=int)
        ref = np.array([], dtype=int)
        for i in range(num_blocks):
            d = np.random.randint(2, size=num_info_bits)
            data = np.append(data, d)
            ref = np.append(ref, python_encoder.encode(d))
        polar_encoder = fec.polar_encoder.make(
            block_size,
            num_info_bits,
            frozen_bit_positions,
            frozen_bit_values,
            is_packed)
        return data, ref, polar_encoder


if __name__ == '__main__':
    gr_unittest.run(test_polar_encoder)
