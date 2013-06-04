#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
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
	pass

    def tearDown(self):
	pass

    def test_001(self):
        self.maxDiff = None;
        tb = gr.top_block()
        src = blocks.vector_source_b( range(0,100) )

        # itemsize, M, N, offset
        km2 = blocks.keep_m_in_n( 1, 1, 2, 0 );
        km3 = blocks.keep_m_in_n( 1, 1, 3, 1 );
        km7 = blocks.keep_m_in_n( 1, 1, 7, 2 );
        snk2 = blocks.vector_sink_b();
        snk3 = blocks.vector_sink_b();
        snk7 = blocks.vector_sink_b();
        tb.connect(src,km2,snk2);
        tb.connect(src,km3,snk3);
        tb.connect(src,km7,snk7);
        tb.run();

        self.assertEqual(range(0,100,2), list(snk2.data()));
        self.assertEqual(range(1,100,3), list(snk3.data()));
        self.assertEqual(range(2,100,7), list(snk7.data()));


if __name__ == '__main__':
    gr_unittest.run(test_keep_m_in_n, "test_keep_m_in_n.xml")

