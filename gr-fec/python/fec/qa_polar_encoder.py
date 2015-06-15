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
from _qa_helper import _qa_helper
import numpy as np

from extended_encoder import extended_encoder
from extended_decoder import extended_decoder
from polar.encoder import PolarEncoder
from polar.helper_functions import get_frozen_bit_positions
from polar.helper_functions import bit_reverse_vector


class test_polar_encoder(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        block_size = 16
        num_info_bits = 8
        frozen_bit_positions = np.arange(block_size - num_info_bits)
        frozen_bit_values = np.array([],)

        polar_encoder = fec.polar_encoder.make(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        self.assertEqual(block_size, polar_encoder.get_output_size())
        self.assertEqual(num_info_bits, polar_encoder.get_input_size())
        self.assertFloatTuplesAlmostEqual((float(num_info_bits) / block_size, ), (polar_encoder.rate(), ))
        self.assertFalse(polar_encoder.set_frame_size(10))

    def test_002_work_function(self):
        block_size = 256
        num_info_bits = 128
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = get_frozen_bit_positions('polar', block_size, num_frozen_bits, 0.11)
        frozen_bit_values = np.array([0] * num_frozen_bits,)
        python_encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        is_packed = False
        polar_encoder = fec.polar_encoder.make(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed)

        data = np.ones(num_info_bits, dtype=int)
        src = blocks.vector_source_b(data, False)
        packer = blocks.pack_k_bits_bb(8)
        enc_block = extended_encoder(polar_encoder, None, '11')
        unpacker = blocks.unpack_k_bits_bb(8)
        snk = blocks.vector_sink_b(1)

        if is_packed:
            self.tb.connect(src, packer, enc_block, unpacker, snk)
        else:
            self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        penc = python_encoder.encode(data)

        print(res)
        print(penc)
        self.assertTupleEqual(tuple(res), tuple(penc))

    def test_003_big_input(self):
        is_packed = False
        num_blocks = 30
        block_size = 256
        num_info_bits = 128
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = get_frozen_bit_positions('/home/johannes/src/gnuradio-polar/gr-fec/python/fec/polar', block_size, num_frozen_bits, 0.11)
        frozen_bit_values = np.array([0] * num_frozen_bits,)
        python_encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        polar_encoder = fec.polar_encoder.make(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values, is_packed)

        data = np.array([], dtype=int)
        ref = np.array([], dtype=int)

        for i in range(num_blocks):
            d = np.random.randint(2, size=num_info_bits)
            data = np.append(data, d)
            ref = np.append(ref, python_encoder.encode(d))


        src = blocks.vector_source_b(data, False)
        packer = blocks.pack_k_bits_bb(8)
        enc_block = extended_encoder(polar_encoder, None, '11')
        unpacker = blocks.unpack_k_bits_bb(8)
        snk = blocks.vector_sink_b(1)

        if is_packed:
            self.tb.connect(src, packer, enc_block, unpacker, snk)
        else:
            self.tb.connect(src, enc_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        # penc = python_encoder.encode(data)

        print(res)
        print(ref)
        self.assertTupleEqual(tuple(res), tuple(ref))





if __name__ == '__main__':
    gr_unittest.run(test_polar_encoder)

