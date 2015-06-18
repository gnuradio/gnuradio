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
from Crypto.Cipher._AES import block_size

from gnuradio import gr, gr_unittest, blocks
import fec_swig as fec
from _qa_helper import _qa_helper
import numpy as np
import os

from extended_encoder import extended_encoder
from extended_decoder import extended_decoder
from polar.encoder import PolarEncoder
from polar.decoder import PolarDecoder
from polar.helper_functions import get_frozen_bit_positions
# from polar.helper_functions import bit_reverse_vector

# print('PID:', os.getpid())
# raw_input('tell me smth')

class test_polar_decoder_sc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        is_packed = False
        block_size = 16
        num_info_bits = 8
        frozen_bit_positions = np.arange(block_size - num_info_bits)
        frozen_bit_values = np.array([],)

        polar_decoder = fec.polar_decoder_sc.make(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed)

        self.assertEqual(num_info_bits, polar_decoder.get_output_size())
        self.assertEqual(block_size, polar_decoder.get_input_size())
        self.assertFloatTuplesAlmostEqual((float(block_size) / num_info_bits, ), (polar_decoder.rate(), ))
        self.assertFalse(polar_decoder.set_frame_size(10))

    def test_002_one_vector(self):
        print "test_002_one_vector"
        is_packed = False
        block_power = 8
        block_size = 2 ** block_power
        num_info_bits = 2 ** (block_power - 1)
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = get_frozen_bit_positions('polar', block_size, num_frozen_bits, 0.11)
        frozen_bit_values = np.array([0] * num_frozen_bits,)
        print frozen_bit_positions

        python_decoder = PolarDecoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        bits = np.ones(num_info_bits, dtype=int)
        # bits = np.random.randint(2, size=num_info_bits)
        encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
        data = encoder.encode(bits)
        # data = np.array([0, 1, 1, 0, 1, 0, 1, 0], dtype=int)
        gr_data = -2.0 * data + 1.0

        polar_decoder = fec.polar_decoder_sc.make(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed)
        src = blocks.vector_source_f(gr_data, False)
        dec_block = extended_decoder(polar_decoder, None)
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, dec_block)
        self.tb.connect(dec_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)

        ref = python_decoder.decode(data)

        print("input:", data)
        print("res  :", res)
        print("ref  :", ref)

        self.assertTupleEqual(tuple(res), tuple(ref))

    def test_003_stream(self):
        print "test_002_stream"
        nframes = 3
        is_packed = False
        block_power = 8
        block_size = 2 ** block_power
        num_info_bits = 2 ** (block_power - 1)
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = get_frozen_bit_positions('polar', block_size, num_frozen_bits, 0.11)
        frozen_bit_values = np.array([0] * num_frozen_bits,)
        print frozen_bit_positions

        python_decoder = PolarDecoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
        encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        bits = np.array([], dtype=int)
        data = np.array([], dtype=int)
        for n in range(nframes):
            b = np.random.randint(2, size=num_info_bits)
            d = encoder.encode(b)
            bits = np.append(bits, b)
            data = np.append(data, d)
        # bits = np.ones(num_info_bits, dtype=int)
        # bits = np.random.randint(2, size=num_info_bits)
        # data = encoder.encode(bits)
        # data = np.array([0, 1, 1, 0, 1, 0, 1, 0], dtype=int)
        gr_data = -2.0 * data + 1.0

        polar_decoder = fec.polar_decoder_sc.make(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed)
        src = blocks.vector_source_f(gr_data, False)
        dec_block = extended_decoder(polar_decoder, None)
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, dec_block)
        self.tb.connect(dec_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)

        # ref = python_decoder.decode(data)

        print("input:", data)
        print("res  :", res)
        # print("ref  :", ref)

        self.assertTupleEqual(tuple(res), tuple(bits))


if __name__ == '__main__':
    gr_unittest.run(test_polar_decoder_sc)


