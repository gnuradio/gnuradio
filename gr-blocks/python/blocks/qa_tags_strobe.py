#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
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
import pmt
import math

class test_tags_strobe(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        N = 10000
        nsamps = 1000
        ntags = N / nsamps - 1

        src = blocks.tags_strobe(gr.sizeof_float,
                                 pmt.intern("TEST"), nsamps)
        hed = blocks.head(gr.sizeof_float, N)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, hed, dst)
        self.tb.run()

        self.assertEqual(ntags, len(dst.tags()))

        n_expected = nsamps
        for tag in dst.tags():
            self.assertEqual(tag.offset, n_expected)
            n_expected += nsamps

    def test_002(self):
        N = 10000
        nsamps = 123
        ntags = N / nsamps

        src = blocks.tags_strobe(gr.sizeof_float,
                                 pmt.intern("TEST"), nsamps)
        hed = blocks.head(gr.sizeof_float, N)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, hed, dst)
        self.tb.run()

        self.assertEqual(ntags, len(dst.tags()))

        n_expected = nsamps
        for tag in dst.tags():
            self.assertEqual(tag.offset, n_expected)
            n_expected += nsamps

    def test_003(self):
        N = 100000
        nsamps = 10000
        ntags = N / nsamps - 1

        src = blocks.tags_strobe(gr.sizeof_float,
                                 pmt.intern("TEST"), nsamps)
        hed = blocks.head(gr.sizeof_float, N)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, hed, dst)
        self.tb.run()

        self.assertEqual(ntags, len(dst.tags()))

        n_expected = nsamps
        for tag in dst.tags():
            self.assertEqual(tag.offset, n_expected)
            n_expected += nsamps

if __name__ == '__main__':
    gr_unittest.run(test_tags_strobe, "test_tags_strobe.xml")
