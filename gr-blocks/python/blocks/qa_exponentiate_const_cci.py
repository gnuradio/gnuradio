#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2017 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import pmt


class qa_exponentiate_const_cci(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        for exponent in range(1, 10):
            in_data = [1 + 1j, -1, 4 - 1j, -3 - 7j]
            out_data = [
                in_data[0]**exponent,
                in_data[1]**exponent,
                in_data[2]**exponent,
                in_data[3]**exponent]

            # Test streaming input
            source = blocks.vector_source_c(in_data, False, 1)
            exponentiate_const_cci = blocks.exponentiate_const_cci(exponent)
            sink = blocks.vector_sink_c(1)

            self.tb.connect(source, exponentiate_const_cci, sink)
            self.tb.run()

            self.assertAlmostEqual(sink.data(), out_data)

            # Test vector input
            for vlen in [2, 4]:
                source = blocks.vector_source_c(in_data, False, 1)
                s2v = blocks.stream_to_vector(gr.sizeof_gr_complex, vlen)
                exponentiate_const_cci = blocks.exponentiate_const_cci(
                    exponent, vlen)
                v2s = blocks.vector_to_stream(gr.sizeof_gr_complex, vlen)
                sink = blocks.vector_sink_c(1)

                self.tb.connect(source, s2v, exponentiate_const_cci, v2s, sink)
                self.tb.run()

                self.assertAlmostEqual(sink.data(), out_data)


if __name__ == '__main__':
    gr_unittest.run(qa_exponentiate_const_cci)
