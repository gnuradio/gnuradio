#!/usr/bin/env python
#
# Copyright 2004,2007,2010-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks

class test_constellation_decoder(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_constellation_decoder_cb_bpsk(self):
        cnst = digital.constellation_bpsk()
        src_data =        (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                           0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j)
        expected_result = (        1,           1,           0,            0,
                                   1,           0,           1)
        src = blocks.vector_source_c(src_data)
        op = digital.constellation_decoder_cb(cnst.base())
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

    def _test_constellation_decoder_cb_qpsk(self):
        cnst = digital.constellation_qpsk()
        src_data =        (0.5 + 0.5j,  0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j,
                           0.8 + 1.0j, -0.5 + 0.1j,  0.1 - 1.2j)
        expected_result = (        3,           1,           0,            2,
                                   3,           2,           1)
        src = blocks.vector_source_c(src_data)
        op = digital.constellation_decoder_cb(cnst.base())
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
        #print "actual result", actual_result
        #print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.run(test_constellation_decoder, "test_constellation_decoder.xml")
