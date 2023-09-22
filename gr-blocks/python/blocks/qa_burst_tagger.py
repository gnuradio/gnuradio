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

import pmt


class test_burst_tagger(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        trg_data = (-1, -1, 1, 1, -1, -1, 1, 1, -1, -1)
        src = blocks.vector_source_i(src_data)
        trg = blocks.vector_source_s(trg_data)
        op = blocks.burst_tagger(gr.sizeof_int)
        snk = blocks.tag_debug(gr.sizeof_int, "burst tagger QA")
        self.tb.connect(src, (op, 0))
        self.tb.connect(trg, (op, 1))
        self.tb.connect(op, snk)
        self.tb.run()

        x = snk.current_tags()
        self.assertEqual(2, x[0].offset)
        self.assertEqual(4, x[1].offset)
        self.assertEqual(6, x[2].offset)
        self.assertEqual(8, x[3].offset)

        self.assertEqual(True, pmt.to_bool(x[0].value))
        self.assertEqual(False, pmt.to_bool(x[1].value))
        self.assertEqual(True, pmt.to_bool(x[2].value))
        self.assertEqual(False, pmt.to_bool(x[3].value))


if __name__ == '__main__':
    gr_unittest.run(test_burst_tagger)
