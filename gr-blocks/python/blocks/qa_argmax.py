#!/usr/bin/env python
#
# Copyright 2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

import math


class test_arg_max(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        tb = self.tb

        src1_data = [0, 0.2, -0.3, 0, 12, 0]
        src2_data = [0, 0.0, 3.0, 0, 10, 0]
        src3_data = [0, 0.0, 3.0, 0, 1, 0]

        src1 = blocks.vector_source_f(src1_data)
        s2v1 = blocks.stream_to_vector(gr.sizeof_float, len(src1_data))
        tb.connect(src1, s2v1)

        src2 = blocks.vector_source_f(src2_data)
        s2v2 = blocks.stream_to_vector(gr.sizeof_float, len(src1_data))
        tb.connect(src2, s2v2)

        src3 = blocks.vector_source_f(src3_data)
        s2v3 = blocks.stream_to_vector(gr.sizeof_float, len(src1_data))
        tb.connect(src3, s2v3)

        dst1 = blocks.vector_sink_s()
        dst2 = blocks.vector_sink_s()
        argmax = blocks.argmax_fs(len(src1_data))

        tb.connect(s2v1, (argmax, 0))
        tb.connect(s2v2, (argmax, 1))
        tb.connect(s2v3, (argmax, 2))

        tb.connect((argmax, 0), dst1)
        tb.connect((argmax, 1), dst2)

        tb.run()
        index = dst1.data()
        source = dst2.data()
        self.assertEqual(index, [4, ])
        self.assertEqual(source, [0, ])


if __name__ == '__main__':
    gr_unittest.run(test_arg_max)
