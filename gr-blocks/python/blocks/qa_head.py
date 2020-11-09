#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_head(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_head(self):
        src_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        expected_result = [1, 2, 3, 4]
        src1 = blocks.vector_source_i(src_data)
        op = blocks.head(gr.sizeof_int, 4)
        dst1 = blocks.vector_sink_i()
        self.tb.connect(src1, op)
        self.tb.connect(op, dst1)
        self.tb.run()
        dst_data = dst1.data()
        self.assertEqual(expected_result, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_head)
