#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

class test_repeat (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_float(self):
        src_data = [n*1.0 for n in range(100)];
        dst_data = []
        for n in range(100):
            dst_data += [1.0*n, 1.0*n, 1.0*n]

        src = blocks.vector_source_f(src_data)
        rpt = blocks.repeat(gr.sizeof_float, 3)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, rpt, dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(dst_data, dst.data(), 6)

if __name__ == '__main__':
    gr_unittest.run(test_repeat, "test_repeat.xml")
