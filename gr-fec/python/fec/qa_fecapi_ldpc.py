#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
import fec_swig as fec
from _qa_helper import _qa_helper

from extended_encoder import extended_encoder
from extended_decoder import extended_decoder

import os

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
        dec = fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())
        threading = None
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_01(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())
        threading = 'ordinary'
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_02(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())
        threading = 'capillary'
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_03(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0058_gen_matrix.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_G_matrix(filename)
        k = LDPC_matrix_object.k()
        enc = fec.ldpc_gen_mtrx_encoder.make(LDPC_matrix_object)
        dec = fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())
        threading = 'capillary'
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_03(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0058_gen_matrix.alist"
        gap = 4
        k = 100 - 58
        enc = fec.ldpc_par_mtrx_encoder.make(filename, gap)
        dec = fec.ldpc_decoder.make(filename)
        threading = 'capillary'
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)), range(0,1))
        dec = map((lambda a: fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())), range(0,1))
        threading = None
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_01(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)), range(0,1))
        dec = map((lambda a: fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())), range(0,1))
        threading = 'ordinary'
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_02(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        enc = map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)), range(0,1))
        dec = map((lambda a: fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())), range(0,1))
        threading = 'capillary'
        self.test = _qa_helper(10*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_03(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 10

        enc = []
        for n in range(0,dims):
            H = fec.ldpc_H_matrix(filename, gap)
            enc.append(fec.ldpc_par_mtrx_encoder.make_H(H))

        dec = []
        for n in range(0,dims):
            H = fec.ldpc_H_matrix(filename, gap)
            dec.append(fec.ldpc_bit_flip_decoder.make(H.get_base_sptr()))

        k = 27
        threading = 'ordinary'
        self.test = _qa_helper(dims*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_04(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 16

        enc = []
        for n in range(0,dims):
            H = fec.ldpc_H_matrix(filename, gap)
            enc.append(fec.ldpc_par_mtrx_encoder.make_H(H))

        dec = []
        for n in range(0,dims):
            H = fec.ldpc_H_matrix(filename, gap)
            dec.append(fec.ldpc_bit_flip_decoder.make(H.get_base_sptr()))


        k = 27
        threading = 'capillary'
        self.test = _qa_helper(dims*k, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_05(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5

        enc = []
        for n in range(0,dims):
            H = fec.ldpc_H_matrix(filename, gap)
            enc.append(fec.ldpc_par_mtrx_encoder.make_H(H))

        dec = []
        for n in range(0,dims):
            H = fec.ldpc_H_matrix(filename, gap)
            dec.append(fec.ldpc_bit_flip_decoder.make(H.get_base_sptr()))

        k = H.k()
        threading = 'capillary'
        self.assertRaises(AttributeError, lambda: extended_encoder(enc, threading=threading, puncpat="11"))

    def test_parallelism1_06(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dec = map((lambda a: fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())), range(0,dims))
        threading = 'capillary'
        self.assertRaises(AttributeError, lambda: extended_decoder(dec, threading=threading, puncpat="11"))

    def test_parallelism2_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dims1 = 16
        dims2 = 16
        enc = map((lambda b: map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)),
                                 range(0,dims1))), range(0,dims2))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_encoder(enc, threading=threading, puncpat="11"))

    def test_parallelism2_00(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dims1 = 16
        dims2 = 16
        enc = map((lambda b: map((lambda a: fec.ldpc_par_mtrx_encoder.make_H(LDPC_matrix_object)),
                                 range(0,dims1))), range(0,dims2))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_encoder(enc, threading=threading, puncpat="11"))

    def test_parallelism2_01(self):
        filename = LDPC_ALIST_DIR + "n_0100_k_0027_gap_04.alist"
        gap = 4
        dims = 5
        LDPC_matrix_object = fec.ldpc_H_matrix(filename, gap)
        k = LDPC_matrix_object.k()
        dims1 = 16
        dims2 = 16
        dec = map((lambda b: map((lambda a: fec.ldpc_bit_flip_decoder.make(LDPC_matrix_object.get_base_sptr())),
                                 range(0,dims1))), range(0,dims2))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_decoder(dec, threading=threading, puncpat="11"))

if __name__ == '__main__':
    gr_unittest.run(test_fecapi_ldpc, "test_fecapi_ldpc.xml")
