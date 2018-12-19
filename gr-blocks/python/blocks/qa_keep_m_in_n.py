#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013,2018 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#


from gnuradio import gr, gr_unittest, blocks

import sys
import random


class test_keep_m_in_n(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)

    def tearDown(self):
        pass

    def test_001(self):
        self.maxDiff = None
        tb = gr.top_block()
        src = blocks.vector_source_c(list(range(0, 100)))

        # itemsize, M, N, offset
        km2 = blocks.keep_m_in_n(8, 1, 2, 0)
        km3 = blocks.keep_m_in_n(8, 1, 3, 1)
        km7 = blocks.keep_m_in_n(8, 1, 7, 2)
        snk2 = blocks.vector_sink_c()
        snk3 = blocks.vector_sink_c()
        snk7 = blocks.vector_sink_c()
        tb.connect(src, km2, snk2)
        tb.connect(src, km3, snk3)
        tb.connect(src, km7, snk7)
        tb.run()

        self.assertEqual(list(range(0, 100, 2)), list(snk2.data()))
        self.assertEqual(list(range(1, 100, 3)), list(snk3.data()))
        self.assertEqual(list(range(2, 100, 7)), list(snk7.data()))

    def test_002(self):
        self.maxDiff = None
        tb = gr.top_block()
        src = blocks.vector_source_f(list(range(0, 100)))

        km = []
        snk = []
        for i in range(5):
            km.append(blocks.keep_m_in_n(4, 3, 5, i))
            snk.append(blocks.vector_sink_f())
            tb.connect(src, km[i], snk[i])
        tb.run()

        for i in range(5):
            self.assertEqual(
                sorted(
                    list(range(i, 100, 5)) +
                    list(range((i+1) % 5, 100, 5)) +
                    list(range((i+2) % 5, 100, 5))
                ),
                list(snk[i].data())
            )

    def test_003(self):
        with self.assertRaises(RuntimeError) as cm:
            blocks.keep_m_in_n(8, 0, 5, 0)
        self.assertEqual(str(cm.exception), 'keep_m_in_n: m=0 but must be > 0')

        with self.assertRaises(RuntimeError) as cm:
            blocks.keep_m_in_n(8, 5, 0, 0)
        self.assertEqual(str(cm.exception), 'keep_m_in_n: n=0 but must be > 0')

        with self.assertRaises(RuntimeError) as cm:
            blocks.keep_m_in_n(8, 6, 5, 0)
        self.assertEqual(str(cm.exception), 'keep_m_in_n: m (6) <= n 5')

        with self.assertRaises(RuntimeError) as cm:
            blocks.keep_m_in_n(8, 2, 5, -1)
        self.assertEqual(str(cm.exception), 'keep_m_in_n: offset (-1) must be >= 0')

        with self.assertRaises(RuntimeError) as cm:
            blocks.keep_m_in_n(8, 2, 5, 5)
        self.assertEqual(str(cm.exception), 'keep_m_in_n: offset (5) < n (5)')


if __name__ == '__main__':
    gr_unittest.run(test_keep_m_in_n, "test_keep_m_in_n.xml")
