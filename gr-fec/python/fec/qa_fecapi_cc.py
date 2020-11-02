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


class test_fecapi_cc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_parallelism0_00(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        enc = fec.cc_encoder_make(frame_size * 8, k, rate, polys)
        dec = fec.cc_decoder.make(frame_size * 8, k, rate, polys)
        threading = None
        self.test = _qa_helper(4 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism0_01(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        enc = fec.cc_encoder_make(frame_size * 8, k, rate, polys)
        dec = fec.cc_decoder.make(frame_size * 8, k, rate, polys)
        threading = 'ordinary'
        self.test = _qa_helper(5 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism0_02(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        enc = fec.cc_encoder_make(frame_size * 8, k, rate, polys)
        dec = fec.cc_decoder.make(frame_size * 8, k, rate, polys)
        threading = 'capillary'
        self.test = _qa_helper(5 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism1_00(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        enc = list(map((lambda a: fec.cc_encoder_make(
            frame_size * 8, k, rate, polys)), list(range(0, 1))))
        dec = list(map((lambda a: fec.cc_decoder.make(
            frame_size * 8, k, rate, polys)), list(range(0, 1))))
        threading = None
        self.test = _qa_helper(5 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism1_01(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        enc = list(map((lambda a: fec.cc_encoder_make(
            frame_size * 8, k, rate, polys)), list(range(0, 1))))
        dec = list(map((lambda a: fec.cc_decoder.make(
            frame_size * 8, k, rate, polys)), list(range(0, 1))))
        threading = 'ordinary'
        self.test = _qa_helper(5 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism1_02(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        enc = list(map((lambda a: fec.cc_encoder_make(
            frame_size * 8, k, rate, polys)), list(range(0, 1))))
        dec = list(map((lambda a: fec.cc_decoder.make(
            frame_size * 8, k, rate, polys)), list(range(0, 1))))
        threading = 'capillary'
        self.test = _qa_helper(5 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism1_03(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        mode = fec.CC_TERMINATED
        enc = list(map((lambda a: fec.cc_encoder_make(
            frame_size * 8, k, rate, polys, mode=mode)), list(range(0, 4))))
        dec = list(map((lambda a: fec.cc_decoder.make(
            frame_size * 8, k, rate, polys, mode=mode)), list(range(0, 4))))
        threading = 'capillary'
        self.test = _qa_helper(4 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism1_04(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        mode = fec.CC_TRUNCATED
        enc = list(map((lambda a: fec.cc_encoder_make(
            frame_size * 8, k, rate, polys, mode=mode)), list(range(0, 4))))
        dec = list(map((lambda a: fec.cc_decoder.make(
            frame_size * 8, k, rate, polys, mode=mode)), list(range(0, 4))))
        threading = 'capillary'
        self.test = _qa_helper(4 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)

    def test_parallelism1_05(self):
        frame_size = 30
        k = 7
        rate = 2
        polys = [109, 79]
        mode = fec.CC_TAILBITING
        enc = list(map((lambda a: fec.cc_encoder_make(
            frame_size * 8, k, rate, polys, mode=mode)), list(range(0, 4))))
        dec = list(map((lambda a: fec.cc_decoder.make(
            frame_size * 8, k, rate, polys, mode=mode)), list(range(0, 4))))
        threading = 'capillary'
        self.test = _qa_helper(4 * frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_out = self.test.snk_output.data()
        data_in = self.test.snk_input.data()[0:len(data_out)]

        self.assertEqual(data_in, data_out)


if __name__ == '__main__':
    gr_unittest.run(test_fecapi_cc)
