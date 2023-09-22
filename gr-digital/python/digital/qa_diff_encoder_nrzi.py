#!/usr/bin/env python
#
# Copyright 2020-2021 Daniel Estevez <daniel@destevez.net>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest, digital, blocks
import numpy as np


class test_nrzi(gr_unittest.TestCase):
    def setUp(self):
        test_size = 256
        self.data = np.random.randint(0, 2, test_size, dtype='uint8')
        self.source = blocks.vector_source_b(self.data, False, 1, [])
        self.sink = blocks.vector_sink_b(1, 0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None
        del(self.data)
        del(self.source)
        del(self.sink)

    def test_encode(self):
        """Performs NRZI encode and checks the result"""
        encoder = digital.diff_encoder_bb(2, digital.DIFF_NRZI)

        self.tb.connect(self.source, encoder, self.sink)
        self.tb.start()
        self.tb.wait()

        expected = np.cumsum((1 ^ self.data) & 1) & 1

        np.testing.assert_equal(
            self.sink.data(), expected,
            'NRZI encode output does not match expected result')

    def test_decode(self):
        """Performs NRZI decode and checks the result"""
        decoder = digital.diff_decoder_bb(2, digital.DIFF_NRZI)

        self.tb.connect(self.source, decoder, self.sink)
        self.tb.start()
        self.tb.wait()

        expected = self.data[1:] ^ self.data[:-1] ^ 1

        np.testing.assert_equal(
            self.sink.data()[1:], expected,
            'NRZI decode output does not match expected result')

    def test_encode_decode(self):
        """Performs NRZI encode and decode and checks the result"""
        encoder = digital.diff_encoder_bb(2, digital.DIFF_NRZI)
        decoder = digital.diff_decoder_bb(2, digital.DIFF_NRZI)

        self.tb.connect(self.source, encoder, decoder, self.sink)
        self.tb.start()
        self.tb.wait()

        np.testing.assert_equal(
            self.sink.data(), self.data,
            'NRZI encoded and decoded output does not match input')


if __name__ == '__main__':
    gr_unittest.run(test_nrzi)
