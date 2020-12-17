#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import math


class test_null_sink_source(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        # Just running some data through null source/sink
        src = blocks.null_source(gr.sizeof_float)
        hed = blocks.head(gr.sizeof_float, 100)
        dst = blocks.null_sink(gr.sizeof_float)

        self.tb.connect(src, hed, dst)
        self.tb.run()


if __name__ == '__main__':
    gr_unittest.run(test_null_sink_source)
