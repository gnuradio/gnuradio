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

from extended_decoder import extended_decoder
from polar.encoder import PolarEncoder
import polar.channel_construction as cc

# import os
# print('PID:', os.getpid())
# raw_input('tell me smth')


class test_polar_decoder_sc_list(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_setup(self):
        block_size = 16
        num_info_bits = 8
        max_list_size = 4
        frozen_bit_positions = np.arange(block_size - num_info_bits)
        frozen_bit_values = np.array([],)

        polar_decoder = fec.polar_decoder_sc_list.make(max_list_size, block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        self.assertEqual(num_info_bits, polar_decoder.get_output_size())
        self.assertEqual(block_size, polar_decoder.get_input_size())
        self.assertFloatTuplesAlmostEqual((float(num_info_bits) / block_size, ), (polar_decoder.rate(), ))
        self.assertFalse(polar_decoder.set_frame_size(10))

    def test_002_one_vector(self):
        print "test_002_one_vector"
        expo = 6
        block_size = 2 ** expo
        num_info_bits = 2 ** (expo - 1)
        max_list_size = 2 ** (expo - 2)
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = cc.frozen_bit_positions(block_size, num_info_bits, 0.0)
        frozen_bit_values = np.array([0] * num_frozen_bits,)

        bits = np.random.randint(2, size=num_info_bits)
        encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
        data = encoder.encode(bits)
        gr_data = 2.0 * data - 1.0

        polar_decoder = fec.polar_decoder_sc_list.make(max_list_size, block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
        src = blocks.vector_source_f(gr_data, False)
        dec_block = extended_decoder(polar_decoder, None)
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, dec_block)
        self.tb.connect(dec_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)

        print("\ninput -> result -> bits")
        print(data)
        print(res)
        print(bits)

        self.assertTupleEqual(tuple(res), tuple(bits))

    def test_003_stream(self):
        print "test_003_stream"
        nframes = 5
        expo = 8
        block_size = 2 ** expo
        num_info_bits = 2 ** (expo - 1)
        max_list_size = 2 ** (expo - 2)
        num_frozen_bits = block_size - num_info_bits
        frozen_bit_positions = cc.frozen_bit_positions(block_size, num_info_bits, 0.0)
        frozen_bit_values = np.array([0] * num_frozen_bits,)

        encoder = PolarEncoder(block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)

        ref = np.array([], dtype=int)
        data = np.array([], dtype=int)
        for i in range(nframes):
            b = np.random.randint(2, size=num_info_bits)
            d = encoder.encode(b)
            data = np.append(data, d)
            ref = np.append(ref, b)
        gr_data = 2.0 * data - 1.0

        polar_decoder = fec.polar_decoder_sc_list.make(max_list_size, block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
        src = blocks.vector_source_f(gr_data, False)
        dec_block = extended_decoder(polar_decoder, None)
        snk = blocks.vector_sink_b(1)

        self.tb.connect(src, dec_block)
        self.tb.connect(dec_block, snk)
        self.tb.run()

        res = np.array(snk.data()).astype(dtype=int)
        self.assertTupleEqual(tuple(res), tuple(ref))


if __name__ == '__main__':
    gr_unittest.run(test_polar_decoder_sc_list, "test_polar_decoder_sc_list.xml")
