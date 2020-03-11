#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import random

from gnuradio import gr, gr_unittest, digital, blocks

class test_binary_slicer_fb(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_binary_slicer_fb(self):
        expected_result = ( 0, 1,  0,  0, 1, 1,  0,  0,  0, 1, 1, 1,  0, 1, 1, 1, 1)
        src_data =        (-1, 1, -1, -1, 1, 1, -1, -1, -1, 1, 1, 1, -1, 1, 1, 1, 1)
        src_data = [s + (1 - random.random()) for s in src_data] # add some noise
        src = blocks.vector_source_f(src_data)
        op = digital.binary_slicer_fb()
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.run(test_binary_slicer_fb, "test_binary_slicer_fb.xml")
