#!/usr/bin/env python
#
# Copyright 2012-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_tag_debug(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        # Just run some data through and make sure it doesn't puke.
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        src = blocks.vector_source_i(src_data)
        op = blocks.tag_debug(gr.sizeof_int, "tag QA")
        self.tb.connect(src, op)
        self.tb.run()
        x = op.current_tags()


if __name__ == '__main__':
    gr_unittest.run(test_tag_debug)
