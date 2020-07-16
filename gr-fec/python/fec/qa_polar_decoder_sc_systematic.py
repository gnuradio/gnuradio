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

from gnuradio.fec.extended_decoder import extended_decoder
from gnuradio.fec.polar.encoder import PolarEncoder
from gnuradio.fec.polar import channel_construction as cc

# import os
# print('PID:', os.getpid())
# raw_input('tell me smth')


class test_polar_decoder_sc_systematic(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        block_size = 16
        num_info_bits = 8
        frozen_bit_positions = np.arange(block_size - num_info_bits)

        polar_decoder = fec.polar_decoder_sc_systematic.make(block_size, num_info_bits, frozen_bit_positions)

        self.assertEqual(num_info_bits, polar_decoder.get_output_size())
        self.assertEqual(block_size, polar_decoder.get_input_size())
        self.assertFloatTuplesAlmostEqual((float(num_info_bits) / block_size, ), (polar_decoder.rate(), ))
        self.assertFalse(polar_decoder.set_frame_size(10))

    def test_002_one_vector(self):
        block_power = 4
        block_size = 2 ** block_power
        num_info_bits = block_size // 2
        frozen_bit_positions = cc.frozen_bit_positions(block_size, num_info_bits, 0.0)

        bits, gr_data = self.generate_test_data(block_size, num_info_bits, frozen_bit_positions, 1, False)

        polar_decoder = fec.polar_decoder_sc_systematic.make(block_size, num_info_bits, frozen_bit_positions)
        src = blocks.vector_source_f(gr_data, False)
        dec_block = extended_decoder(polar_decoder, None)
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, dec_block)
        self.tb.connect(dec_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(bits))

    def test_003_stream(self):
        nframes = 3
        block_power = 8
        block_size = 2 ** block_power
        num_info_bits = block_size // 2
        frozen_bit_positions = cc.frozen_bit_positions(block_size, num_info_bits, 0.0)

        bits, gr_data = self.generate_test_data(block_size, num_info_bits, frozen_bit_positions, nframes, False)

        polar_decoder = fec.polar_decoder_sc_systematic.make(block_size, num_info_bits, frozen_bit_positions)
        src = blocks.vector_source_f(gr_data, False)
        dec_block = extended_decoder(polar_decoder, None)
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, dec_block)
        self.tb.connect(dec_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(bits))

    def generate_test_data(self, block_size, num_info_bits, frozen_bit_positions, nframes, onlyones):
        frozen_bit_values = np.zeros(block_size - num_info_bits, dtype=int)
        encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
        bits = np.array([], dtype=int)
        data = np.array([], dtype=int)
        for n in range(nframes):
            if onlyones:
                b = np.ones(num_info_bits, dtype=int)
            else:
                b = np.random.randint(2, size=num_info_bits)
            d = encoder.encode_systematic(b)
            bits = np.append(bits, b)
            data = np.append(data, d)
        gr_data = 2.0 * data - 1.0
        return bits, gr_data


if __name__ == '__main__':
    gr_unittest.run(test_polar_decoder_sc_systematic)
