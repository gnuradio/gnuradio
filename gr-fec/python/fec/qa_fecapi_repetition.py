#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest
from gnuradio import fec

from _qa_helper import _qa_helper


class test_fecapi_repetition(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_parallelism0_00(self):
        frame_size = 30
        rep = 3
        enc = fec.repetition_encoder_make(frame_size * 8, rep)
        dec = fec.repetition_decoder.make(frame_size * 8, rep)
        threading = None
        self.test = _qa_helper(10 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism0_01(self):
        frame_size = 30
        rep = 3
        enc = fec.repetition_encoder_make(frame_size * 8, rep)
        dec = fec.repetition_decoder.make(frame_size * 8, rep)
        threading = 'ordinary'
        self.test = _qa_helper(10 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism0_02(self):
        frame_size = 30
        rep = 3
        enc = fec.repetition_encoder_make(frame_size * 8, rep)
        dec = fec.repetition_decoder.make(frame_size * 8, rep)
        threading = 'capillary'
        self.test = _qa_helper(10 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism1_00(self):
        frame_size = 30
        rep = 3
        enc = list(map((lambda a: fec.repetition_encoder_make(
            frame_size * 8, rep)), list(range(0, 1))))
        dec = list(map((lambda a: fec.repetition_decoder.make(
            frame_size * 8, rep)), list(range(0, 1))))
        threading = None
        self.test = _qa_helper(10 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism1_01(self):
        frame_size = 30
        rep = 3
        enc = list(map((lambda a: fec.repetition_encoder_make(
            frame_size * 8, rep)), list(range(0, 1))))
        dec = list(map((lambda a: fec.repetition_decoder.make(
            frame_size * 8, rep)), list(range(0, 1))))
        threading = 'ordinary'
        self.test = _qa_helper(10 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism1_02(self):
        frame_size = 300
        rep = 3
        enc = list(map((lambda a: fec.repetition_encoder_make(
            frame_size * 8, rep)), list(range(0, 1))))
        dec = list(map((lambda a: fec.repetition_decoder.make(
            frame_size * 8, rep)), list(range(0, 1))))
        threading = 'capillary'
        self.test = _qa_helper(10 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism1_03(self):
        frame_size = 30
        rep = 3
        dims = 10
        enc = list(map((lambda a: fec.repetition_encoder_make(
            frame_size * 8, rep)), list(range(0, dims))))
        dec = list(map((lambda a: fec.repetition_decoder.make(
            frame_size * 8, rep)), list(range(0, dims))))
        threading = 'ordinary'
        self.test = _qa_helper(dims * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)

    def test_parallelism1_04(self):
        frame_size = 30
        rep = 3
        dims = 16
        enc = list(map((lambda a: fec.repetition_encoder_make(
            frame_size * 8, rep)), list(range(0, dims))))
        dec = list(map((lambda a: fec.repetition_decoder.make(
            frame_size * 8, rep)), list(range(0, dims))))
        threading = 'capillary'
        self.test = _qa_helper(dims * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertSequenceEqualGR(data_in, data_out)


if __name__ == '__main__':
    gr_unittest.run(test_fecapi_repetition)
