#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
from gnuradio import blocks, analog
import numpy as np


class qa_random_uniform_source(gr_unittest.TestCase):
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_byte(self):
        minimum = 0
        maximum = 5
        seed = 3
        n_items = 10000
        rnd_src = analog.random_uniform_source_b(minimum, maximum, seed)
        head = blocks.head(1, n_items)
        snk = blocks.vector_sink_b(1)
        self.tb.connect(rnd_src, head, snk)
        # set up fg
        self.tb.run()
        # check data
        res = snk.data()
        self.assertGreaterEqual(minimum, np.min(res))
        self.assertLess(np.max(res), maximum)

    def test_002_short(self):
        minimum = 42
        maximum = 1025
        seed = 3
        n_items = 10000
        rnd_src = analog.random_uniform_source_s(minimum, maximum, seed)
        head = blocks.head(2, n_items)
        snk = blocks.vector_sink_s(1)
        self.tb.connect(rnd_src, head, snk)
        # set up fg
        self.tb.run()
        # check data
        res = snk.data()
        self.assertGreaterEqual(minimum, np.min(res))
        self.assertLess(np.max(res), maximum)

    def test_003_int(self):
        minimum = 2 ** 12 - 2
        maximum = 2 ** 17 + 5
        seed = 3
        n_items = 10000
        rnd_src = analog.random_uniform_source_i(minimum, maximum, seed)
        head = blocks.head(4, n_items)
        snk = blocks.vector_sink_i(1)
        self.tb.connect(rnd_src, head, snk)
        # set up fg
        self.tb.run()
        # check data
        res = snk.data()
        # plt.hist(res)
        # plt.show()

        self.assertGreaterEqual(np.min(res), minimum)
        self.assertLess(np.max(res), maximum)


if __name__ == '__main__':
    gr_unittest.run(qa_random_uniform_source)
