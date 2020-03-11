#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, analog, blocks, channels
import math

class test_fading_model(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        fs = 1000        # baseband sampling rate
        freq = 100

        fDTs = 0.01
        K = 4
        signal = analog.sig_source_c(fs, analog.GR_SIN_WAVE, freq, 1)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = channels.fading_model(8, fDTs=fDTs, LOS=True,
                                   K=K, seed=0)
        snk = blocks.vector_sink_c()
        snk1 = blocks.vector_sink_c()

        self.assertAlmostEqual(K, op.K(), 4)
        self.assertAlmostEqual(fDTs, op.fDTs(), 4)

        #self.tb.connect(signal, head, op, snk)
        #self.tb.connect(op, snk1)
        #self.tb.run()

        #dst_data = snk.data()
        #exp_data = snk1.data()
        #self.assertComplexTuplesAlmostEqual(exp_data, dst_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_fading_model, "test_fading_model.xml")
