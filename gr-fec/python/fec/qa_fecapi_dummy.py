#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import numpy as np

from gnuradio import gr, gr_unittest, blocks
from gnuradio import fec
from gnuradio.fec import extended_encoder
from gnuradio.fec import extended_decoder

from _qa_helper import _qa_helper


class test_fecapi_dummy(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_parallelism0_00(self):
        frame_size = 30
        enc = fec.dummy_encoder_make(frame_size*8)
        dec = fec.dummy_decoder.make(frame_size*8)
        threading = None
        self.test = _qa_helper(10*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_01(self):
        frame_size = 30
        enc = fec.dummy_encoder_make(frame_size*8)
        dec = fec.dummy_decoder.make(frame_size*8)
        threading = 'ordinary'
        self.test = _qa_helper(10*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism0_02(self):
        frame_size = 30
        enc = fec.dummy_encoder_make(frame_size*8)
        dec = fec.dummy_decoder.make(frame_size*8)
        threading = 'capillary'
        self.test = _qa_helper(10*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_00(self):
        frame_size = 30
        enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,1))))
        dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), list(range(0,1))))
        threading = None
        self.test = _qa_helper(10*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_01(self):
        frame_size = 30
        enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,1))))
        dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), list(range(0,1))))
        threading = 'ordinary'
        self.test = _qa_helper(10*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_02(self):
        frame_size = 300
        enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,1))))
        dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), list(range(0,1))))
        threading = 'capillary'
        self.test = _qa_helper(10*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_03(self):
        frame_size = 30
        dims = 10
        enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,dims))))
        dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), list(range(0,dims))))
        threading = 'ordinary'
        self.test = _qa_helper(dims*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_04(self):
        frame_size = 30
        dims = 16
        enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,dims))))
        dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), list(range(0,dims))))
        threading = 'capillary'
        self.test = _qa_helper(dims*frame_size, enc, dec, threading)
        self.tb.connect(self.test)
        self.tb.run()

        data_in = self.test.snk_input.data()
        data_out =self.test.snk_output.data()

        self.assertEqual(data_in, data_out)

    def test_parallelism1_05(self):
        frame_size = 30
        dims = 5
        enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,dims))))
        #dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), range(0,dims)))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_encoder(enc, threading=threading, puncpat="11"))

    def test_parallelism1_06(self):
        frame_size = 30
        dims = 5
        #enc = list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), range(0,dims)))
        dec = list(map((lambda a: fec.dummy_decoder.make(frame_size*8)), list(range(0,dims))))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_decoder(dec, threading=threading, puncpat="11"))

    def test_parallelism2_00(self):
        frame_size = 30
        dims1 = 16
        dims2 = 16
        enc = list(map((lambda b: list(map((lambda a: fec.dummy_encoder_make(frame_size*8)), list(range(0,dims1))))), list(range(0,dims2))))
        #dec = list(map((lambda b: map((lambda a: fec.dummy_decoder_make(frame_size*8)), range(0,dims1))), range(0,dims2)))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_encoder(enc, threading=threading, puncpat="11"))

    def test_parallelism2_01(self):
        frame_size = 30
        dims1 = 16
        dims2 = 16
        dec = list(map((lambda b: list(map((lambda a: fec.dummy_decoder_make(frame_size*8)), list(range(0,dims1))))), list(range(0,dims2))))
        threading = 'capillary'

        self.assertRaises(AttributeError, lambda: extended_decoder(dec, threading=threading, puncpat="11"))

    def test_extended_pack_data(self):
        # test if extended encoder gets correct values for input and output conversion.
        n_frames = 10
        frame_size = 32

        data = np.random.randint(0, 2, n_frames * frame_size)
        data.dtype = np.uint8
        packed_data = np.packbits(data)

        tb = gr.top_block()

        src = blocks.vector_source_b(data)
        snk0 = blocks.vector_sink_b(1)
        snk1 = blocks.vector_sink_b(1)
        snk2 = blocks.vector_sink_b(1)
        snk3 = blocks.vector_sink_b(1)

        packer = blocks.pack_k_bits_bb(8)
        tb.connect(src, packer, snk0)

        enc_unpacked = fec.dummy_encoder_make(frame_size, False, False)
        ext_enc_unp = extended_encoder(enc_unpacked, threading='none', puncpat='11')
        tb.connect(src, ext_enc_unp, snk1)

        enc_pack = fec.dummy_encoder_make(frame_size // 8, True, False)
        ext_enc_pack = extended_encoder(enc_pack, threading='none', puncpat='11')
        tb.connect(src, ext_enc_pack, snk2)

        enc_packed_bits = fec.dummy_encoder_make(frame_size // 8, False, True)
        ext_enc_packed_bits = extended_encoder(enc_packed_bits, threading='none', puncpat='11')
        tb.connect(packer, ext_enc_packed_bits, snk3)

        tb.run()

        r0 = snk0.data()
        r1 = snk1.data()
        r2 = snk2.data()
        r3 = snk3.data()

        data = list(data)
        packed_data = list(packed_data)
        self.assertListEqual(packed_data, r0)
        self.assertListEqual(data, r1)
        self.assertListEqual(packed_data, r2)
        self.assertListEqual(data, r3)


if __name__ == '__main__':
    gr_unittest.run(test_fecapi_dummy, "test_fecapi_dummy.xml")
