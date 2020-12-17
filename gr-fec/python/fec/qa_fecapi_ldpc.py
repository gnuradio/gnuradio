#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import os

from gnuradio import gr, gr_unittest
from gnuradio import fec
from gnuradio.fec import extended_encoder
from gnuradio.fec import extended_decoder

from _qa_helper import _qa_helper


# Get location of the alist files. If run in 'ctest' or 'make test',
# the shell script sets srcdir. Otherwise, we assume we're running
# from the current directory and know where to go.
LDPC_ALIST_DIR = os.getenv('srcdir', '.') + "/../../ldpc_alist/"


class test_fecapi_ldpc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_parallelism0_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())
        threading = None
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_01(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())
        threading = 'ordinary'
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_02(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())
        threading = 'capillary'
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_03(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0058_gen_matrix.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_G_matrix(filename)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_gen_mtrx_encoder.make(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())
        threading = 'capillary'
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_03(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0058_gen_matrix.alist"
        gap = 4
        k = 100 - 58
        enc = fec.ldpc_par_mtrx_encoder.make(filename, gap)
        dec = fec.ldpc_decoder.make(filename)
        threading = 'capillary'
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = list(map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(
            LDPC_matrix_object)), list(range(0, 1))))
        dec = list(map((lambda a: fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())), list(range(0, 1))))
        threading = None
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_01(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = list(map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(
            LDPC_matrix_object)), list(range(0, 1))))
        dec = list(map((lambda a: fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())), list(range(0, 1))))
        threading = 'ordinary'
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_02(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = list(map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(
            LDPC_matrix_object)), list(range(0, 1))))
        dec = list(map((lambda a: fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())), list(range(0, 1))))
        threading = 'capillary'
        self.test = _qa_helper(10 * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_03(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 10

        enc = []
        for n in range(0, dims):
            H = fec.ldpc_H_matrix(filename, gap)
            enc.append(fec.ldpc_par_mtrx_encoder.make_H(H))

        dec = []
        for n in range(0, dims):
            H = fec.ldpc_H_matrix(filename, gap)
            dec.append(fec.ldpc_bit_flip_decoder.make(H.get_base_sptr()))

        k = 27
        threading = 'ordinary'
        self.test = _qa_helper(dims * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_04(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 16

        enc = []
        for n in range(0, dims):
            H = fec.ldpc_H_matrix(filename, gap)
            enc.append(fec.ldpc_par_mtrx_encoder.make_H(H))

        dec = []
        for n in range(0, dims):
            H = fec.ldpc_H_matrix(filename, gap)
            dec.append(fec.ldpc_bit_flip_decoder.make(H.get_base_sptr()))

        k = 27
        threading = 'capillary'
        self.test = _qa_helper(dims * k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out = self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_05(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5

        enc = []
        for n in range(0, dims):
            H = fec.ldpc_H_matrix(filename, gap)
            enc.append(fec.ldpc_par_mtrx_encoder.make_H(H))

        dec = []
        for n in range(0, dims):
            H = fec.ldpc_H_matrix(filename, gap)
            dec.append(fec.ldpc_bit_flip_decoder.make(H.get_base_sptr()))

        k = H.k()
        threading = 'capillary'
        self.assertRaises(
            AttributeError,
            lambda: extended_encoder(
                enc,
                threading=threading,
                puncpat="11"))

    def test_parallelism1_06(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dec = list(map((lambda a: fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())), list(range(0, dims))))
        threading = 'capillary'
        self.assertRaises(
            AttributeError,
            lambda: extended_decoder(
                dec,
                threading=threading,
                puncpat="11"))

    def test_parallelism2_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dims1 = 16
        dims2 = 16
        enc = list(map((lambda b: list(map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(
            LDPC_matrix_object)), list(range(0, dims1))))), list(range(0, dims2))))
        threading = 'capillary'

        self.assertRaises(
            AttributeError,
            lambda: extended_encoder(
                enc,
                threading=threading,
                puncpat="11"))

    def test_parallelism2_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dims1 = 16
        dims2 = 16
        enc = list(map((lambda b: list(map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(
            LDPC_matrix_object)), list(range(0, dims1))))), list(range(0, dims2))))
        threading = 'capillary'

        self.assertRaises(
            AttributeError,
            lambda: extended_encoder(
                enc,
                threading=threading,
                puncpat="11"))

    def test_parallelism2_01(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dims1 = 16
        dims2 = 16
        dec = list(map((lambda b: list(map((lambda a: fec.ldpc_bit_flip_decoder.make(
            LDPC_matrix_object.get_base_sptr())), list(range(0, dims1))))), list(range(0, dims2))))
        threading = 'capillary'

        self.assertRaises(
            AttributeError,
            lambda: extended_decoder(
                dec,
                threading=threading,
                puncpat="11"))


if __name__ == '__main__':
    gr_unittest.run(test_fecapi_ldpc)
