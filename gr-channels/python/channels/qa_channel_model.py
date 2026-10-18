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


class test_channel_model(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        N = 1000         # number of samples to use
        fs = 1000        # baseband sampling rate
        freq = 100

        signal = analog.sig_source_c(fs, analog.GR_SIN_WAVE, freq, 1)
        head = blocks.head(gr.sizeof_gr_complex, N)
        op = channels.channel_model(0.0, 0.0, 1.0, [1, ], 0)
        snk = blocks.vector_sink_c()
        snk1 = blocks.vector_sink_c()

        op.set_noise_voltage(0.0)
        op.set_frequency_offset(0.0)
        op.set_taps([1, ])
        op.set_timing_offset(1.0)

        self.tb.connect(signal, head, op, snk)
        self.tb.connect(op, snk1)
        self.tb.run()

        dst_data = snk.data()
        exp_data = snk1.data()
        self.assertComplexTuplesAlmostEqual(exp_data, dst_data, 5)

    def test_001_cfo_model_std_dev(self):
        op = channels.cfo_model(1e6, 1.0, 100.0, 1)
        self.assertAlmostEqual(op.std_dev(), 1.0)
        self.assertAlmostEqual(op.max_dev(), 100.0)

    def test_002_dynamic_channel_model_setters(self):
        op = channels.dynamic_channel_model(
            1e6, 0.01, 50, 0.5, 100, 8, 5.0, False, 4.0,
            [0.0, 0.9, 1.7], [1, 0.8, 0.3], 8, 0.0, 1)
        op.set_sro_dev_std(0.5)
        self.assertAlmostEqual(op.sro_dev_std(), 0.5)
        self.assertAlmostEqual(op.sro_dev_max(), 50)
        self.assertAlmostEqual(op.cfo_dev_std(), 0.5)
        op.set_samp_rate(2e6)
        self.assertAlmostEqual(op.samp_rate(), 2e6)
        self.assertAlmostEqual(op.doppler_freq(), 5.0, 4)


if __name__ == '__main__':
    gr_unittest.run(test_channel_model)
