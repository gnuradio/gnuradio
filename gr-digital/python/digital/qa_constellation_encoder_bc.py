#!/usr/bin/env python
#
# Copyright 2004,2007,2010-2013,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks
import numpy as np


class test_constellation_encoder(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_constellation_encoder_bc_bpsk(self):
        cnst = digital.constellation_bpsk()

        src_data = (1, 1, 0, 0,
                    1, 0, 1)
        const_map = [-1.0, 1.0]
        expected_result = [const_map[x] for x in src_data]

        src = blocks.vector_source_b(src_data)
        op = digital.constellation_encoder_bc(cnst.base())
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
        # print "actual result", actual_result
        # print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

    def test_constellation_encoder_bc_qpsk(self):
        cnst = digital.constellation_qpsk()
        src_data = (3, 1, 0, 2,
                    3, 2, 1)
        expected_result = [cnst.points()[x] for x in src_data]
        src = blocks.vector_source_b(src_data)
        op = digital.constellation_encoder_bc(cnst.base())
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
        # print "actual result", actual_result
        # print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)

    def test_constellation_encoder_bc_qpsk_random(self):
        cnst = digital.constellation_qpsk()
        src_data = np.random.randint(0, 4, size=20000)
        expected_result = [cnst.points()[x] for x in src_data]
        src = blocks.vector_source_b(src_data)
        op = digital.constellation_encoder_bc(cnst.base())
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
        # print "actual result", actual_result
        # print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.run(test_constellation_encoder)
