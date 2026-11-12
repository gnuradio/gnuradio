#!/usr/bin/env python
#
# Copyright 2026 Brett Gottula
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

import numpy as np

from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio import blocks, analog, digital

from _qa_helper import _qa_helper

FRAME_SIZE = 128
K = 7
RATE = 2
POLYS = [109, 79]


class test_cc_decoder_streaming_lookahead(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_default_lookahead_unchanged(self):
        # Decoding with the default (minimum) lookahead must be bit-exact
        # on a noiseless channel.
        frame_size = 30
        enc = fec.cc_encoder_make(frame_size * 8, K, RATE, POLYS)
        dec = fec.cc_decoder.make(frame_size * 8, K, RATE, POLYS)
        self.test = _qa_helper(5 * frame_size, enc, dec, None)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]
        self.assertEqual(data_in, data_out)

    def test_larger_lookahead_still_bit_exact(self):
        # A non-default lookahead must still decode a noiseless channel
        # bit-exact; only its behavior under noise should change.
        frame_size = 30
        enc = fec.cc_encoder_make(frame_size * 8, K, RATE, POLYS)
        dec = fec.cc_decoder.make(frame_size * 8, K, RATE, POLYS,
                                  mode=fec.CC_STREAMING, lookahead=35)
        self.test = _qa_helper(5 * frame_size, enc, dec, None)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]
        self.assertEqual(data_in, data_out)

    def test_lookahead_below_minimum_rejected(self):
        with self.assertRaises(ValueError):
            fec.cc_decoder.make(FRAME_SIZE, K, RATE, POLYS,
                                mode=fec.CC_STREAMING, lookahead=K - 2)

    def test_lookahead_narrows_frame_edge_ber(self):
        # At the minimum lookahead (k - 1), a CC_STREAMING decoder commits to
        # the bits nearest each frame boundary before the Viterbi survivor
        # paths have had a chance to converge, so those bits carry an
        # elevated error rate. A larger lookahead gives the survivor paths
        # more distance to converge before a decision is committed. Both
        # lookaheads decode the same noise realization here (same seed), so
        # a larger lookahead must produce markedly fewer edge errors on it.
        edge_errs_min_lookahead = self._edge_error_count(lookahead=K - 1)
        edge_errs_large_lookahead = self._edge_error_count(lookahead=35)

        # Sanity check that this operating point actually produces edge
        # errors worth comparing.
        self.assertGreater(edge_errs_min_lookahead, 20)
        self.assertLess(edge_errs_large_lookahead, edge_errs_min_lookahead / 2)

    def _edge_error_count(self, lookahead, num_frames=4000, esno_db=0.0,
                          seed=42, edge_width=15):
        # Encode/decode num_frames random frames over an AWGN channel at the
        # given Es/No and return the number of bit errors in the first/last
        # edge_width bits of each frame.
        enc = fec.cc_encoder_make(FRAME_SIZE, K, RATE, POLYS, mode=fec.CC_STREAMING)
        dec = fec.cc_decoder.make(FRAME_SIZE, K, RATE, POLYS,
                                  mode=fec.CC_STREAMING, lookahead=lookahead)

        tb = gr.top_block()
        rng = np.random.default_rng(1234)
        bits = rng.integers(0, 2, size=FRAME_SIZE * num_frames, dtype=np.uint8)

        src = blocks.vector_source_b(bits.tolist(), False)
        encoder = fec.extended_encoder(enc, threading=None, puncpat='11')
        mapper = digital.map_bb([-1, 1])
        b2f = blocks.char_to_float(1)
        noise_amp = (10.0 ** (-esno_db / 10.0) / 2.0) ** 0.5
        noise = analog.noise_source_f(analog.GR_GAUSSIAN, noise_amp, seed)
        add = blocks.add_ff(1)
        decoder = fec.extended_decoder(dec, threading=None, ann=None, puncpat='11',
                                       integration_period=10000)
        snk_in = blocks.vector_sink_b()
        snk_out = blocks.vector_sink_b()

        tb.connect(src, snk_in)
        tb.connect(src, encoder, mapper, b2f, (add, 0))
        tb.connect(noise, (add, 1))
        tb.connect(add, decoder, snk_out)
        tb.run()

        din = np.array(snk_in.data(), dtype=np.uint8)
        dout = np.array(snk_out.data(), dtype=np.uint8)
        n = min(len(din), len(dout))
        n -= n % FRAME_SIZE
        din = din[:n].reshape(-1, FRAME_SIZE)
        dout = dout[:n].reshape(-1, FRAME_SIZE)
        errs_by_position = (din != dout).sum(axis=0)
        return int(errs_by_position[:edge_width].sum() + errs_by_position[-edge_width:].sum())


if __name__ == '__main__':
    gr_unittest.run(test_cc_decoder_streaming_lookahead)
