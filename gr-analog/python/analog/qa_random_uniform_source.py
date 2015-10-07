#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
    gr_unittest.run(qa_random_uniform_source, "qa_random_uniform_source.xml")
