#!/usr/bin/env python
#
# Copyright 2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks

class test_pn_correlator_cc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_make(self):
        c = digital.pn_correlator_cc(10)

    def test_001_correlate(self):
        degree = 10
        length = 2**degree-1
        src = digital.glfsr_source_f(degree)
        head = blocks.head(gr.sizeof_float, length*length)
        f2c = blocks.float_to_complex()
        corr = digital.pn_correlator_cc(degree)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, head, f2c, corr, dst)
        self.tb.run()
        data = dst.data()
        self.assertEqual(data[-1], (1.0+0j))

if __name__ == '__main__':
    gr_unittest.run(test_pn_correlator_cc, "test_pn_correlator_cc.xml")
