#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import cmath

from gnuradio import gr, gr_unittest, analog, blocks


class test_quadrature_demod(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_quad_demod_001(self):
        f = 1000.0
        fs = 8000.0

        src_data = []
        for i in range(200):
            ti = i / fs
            src_data.append(cmath.exp(2j * cmath.pi * f * ti))

        # f/fs is a quarter turn per sample.
        # Set the gain based on this to get 1 out.
        gain = 1.0 / (cmath.pi / 4)

        expected_result = [0, ] + 199 * [1.0]

        src = blocks.vector_source_c(src_data)
        op = analog.quadrature_demod_cf(gain)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 5)

    def test_fm_end_to_end_002(self):
        f = 1000.0
        fs = 8000.0
        src = analog.sig_source_f(fs, analog.gr_waveform_t.GR_COS_WAVE, f, 1.0)
        head = blocks.head(gr.sizeof_float, 100)
        sensitivity = cmath.pi / 4
        fm = analog.frequency_modulator_fc(sensitivity)
        quad_demod = analog.quadrature_demod_cf(1.0 / sensitivity)
        sink_signal = blocks.vector_sink_f()
        sink_demod = blocks.vector_sink_f()
        self.tb.connect(src, head, sink_signal)
        self.tb.connect(head, fm, quad_demod, sink_demod)
        self.tb.run()
        expected_result = sink_signal.data()
        expected_result[0] = 0.0
        result_data = sink_demod.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_quadrature_demod)
