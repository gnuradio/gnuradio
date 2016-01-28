#!/usr/bin/env python
#
# Copyright 2016 Free Software Foundation, Inc.
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

class test_block_behavior(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        '''
        Tests the max noutput sizes set by the scheduler. When creating
        the block, there is no block_detail and so the max buffer size
        is 0. When the top_block is run, it builds the detail and
        buffers and sets the max value. test_0001 tests when the
        max_noutput_items is set by hand.

        '''

        src = blocks.null_source(gr.sizeof_float)
        op  = blocks.head(gr.sizeof_float, 100)
        snk = blocks.null_sink(gr.sizeof_float)

        maxn_pre = op.max_noutput_items()

        self.tb.connect(src, op, snk)
        self.tb.run()

        maxn_post = op.max_noutput_items()

        self.assertEqual(maxn_pre, 0)
        self.assertEqual(maxn_post, 16384)

    def test_001(self):
        '''
        Tests the max noutput size when being explicitly set.
        '''

        src = blocks.null_source(gr.sizeof_float)
        op  = blocks.head(gr.sizeof_float, 100)
        snk = blocks.null_sink(gr.sizeof_float)

        op.set_max_noutput_items(1024)

        maxn_pre = op.max_noutput_items()

        self.tb.connect(src, op, snk)
        self.tb.run()

        maxn_post = op.max_noutput_items()

        self.assertEqual(maxn_pre, 1024)
        self.assertEqual(maxn_post, 1024)

if __name__ == '__main__':
    gr_unittest.run(test_block_behavior, "test_block_behavior.xml")
