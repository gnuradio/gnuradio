#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks

class test_diff_phasor(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_diff_phasor_cc(self):
        src_data = (0+0j, 1+0j, -1+0j, 3+4j, -3-4j, -3+4j)
        expected_result = (0+0j, 0+0j, -1+0j, -3-4j, -25+0j, -7-24j)
        src = blocks.vector_source_c(src_data)
        op = digital.diff_phasor_cc()
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish
        actual_result = dst.data()  # fetch the contents of the sink
        self.assertComplexTuplesAlmostEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_diff_phasor, "test_diff_phasor.xml")

