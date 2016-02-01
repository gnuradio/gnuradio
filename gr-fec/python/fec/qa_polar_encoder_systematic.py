#!/usr/bin/env python
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest, blocks
import fec_swig as fec
import numpy as np

from extended_encoder import extended_encoder
from polar.encoder import PolarEncoder
import polar.channel_construction as cc

# import os
# print('PID:', os.getpid())
# raw_input('tell me smth')


class test_polar_encoder_systematic(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        block_size = 16
        num_info_bits = 8
        frozen_bit_positions = cc.frozen_bit_positions(block_size, num_info_bits, 0.0)

        polar_encoder = fec.polar_encoder_systematic.make(block_size, num_info_bits, frozen_bit_positions)

        self.assertEqual(block_size, polar_encoder.get_output_size())
        self.assertEqual(num_info_bits, polar_encoder.get_input_size())
        self.assertFloatTuplesAlmostEqual((float(num_info_bits) / block_size, ), (polar_encoder.rate(), ))
        self.assertFalse(polar_encoder.set_frame_size(10))

    def test_002_work_function_packed(self):
        block_size = 256
        num_info_bits = block_size // 2

        data, ref, polar_encoder = self.get_test_data(block_size, num_info_bits, 1)
        src = blocks.vector_source_b(data, False)
        enc_block = extended_encoder(polar_encoder, None, '11')
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_004_big_input(self):
        num_blocks = 30
        block_size = 1024
        num_info_bits = block_size // 8

        data, ref, polar_encoder = self.get_test_data(block_size, num_info_bits, num_blocks)
        src = blocks.vector_source_b(data, False)
        enc_block = extended_encoder(polar_encoder, None, '11')
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(ref))

    def get_test_data(self, block_size, num_info_bits, num_blocks):
        # helper function to set up test data and together with encoder object.
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = cc.frozen_bit_positions(block_size, num_info_bits, 0.0)
        frozen_bit_values = np.array([0] * num_frozen_bits,)
        python_encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        data = np.array([], dtype=int)
        ref = np.array([], dtype=int)
        for i in range(num_blocks):
            d = np.random.randint(2, size=num_info_bits)
            data = np.append(data, d)
            ref = np.append(ref, python_encoder.encode_systematic(d))
        polar_encoder = fec.polar_encoder_systematic.make(block_size, num_info_bits, frozen_bit_positions)
        return data, ref, polar_encoder


if __name__ == '__main__':
    gr_unittest.run(test_polar_encoder_systematic)


