#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import random

from gnuradio import gr, gr_unittest, digital, blocks

def make_random_int_tuple(L, min, max):
    result = []
    for x in range(L):
        result.append(random.randint(min, max))
    return tuple(result)


class test_diff_encoder(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_diff_encdec_000(self):
        random.seed(0)
        modulus = 2
        src_data = make_random_int_tuple(1000, 0, modulus-1)
        expected_result = src_data
        src = blocks.vector_source_b(src_data)
        enc = digital.diff_encoder_bb(modulus)
        dec = digital.diff_decoder_bb(modulus)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, enc, dec, dst)
        self.tb.run()               # run the graph and wait for it to finish
        actual_result = dst.data()  # fetch the contents of the sink
        self.assertEqual(expected_result, actual_result)

    def test_diff_encdec_001(self):
        random.seed(0)
        modulus = 4
        src_data = make_random_int_tuple(1000, 0, modulus-1)
        expected_result = src_data
        src = blocks.vector_source_b(src_data)
        enc = digital.diff_encoder_bb(modulus)
        dec = digital.diff_decoder_bb(modulus)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, enc, dec, dst)
        self.tb.run()               # run the graph and wait for it to finish
        actual_result = dst.data()  # fetch the contents of the sink
        self.assertEqual(expected_result, actual_result)

    def test_diff_encdec_002(self):
        random.seed(0)
        modulus = 8
        src_data = make_random_int_tuple(40000, 0, modulus-1)
        expected_result = src_data
        src = blocks.vector_source_b(src_data)
        enc = digital.diff_encoder_bb(modulus)
        dec = digital.diff_decoder_bb(modulus)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, enc, dec, dst)
        self.tb.run()               # run the graph and wait for it to finish
        actual_result = dst.data()  # fetch the contents of the sink
        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_diff_encoder, "test_diff_encoder.xml")

