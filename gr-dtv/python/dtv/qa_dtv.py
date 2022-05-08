#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, digital, dtv

import os
import pmt
from os.path import getsize
import numpy as np

ts_in_file = os.path.join(os.getenv("srcdir"), "vv009-4kfft.ts")
complex_out_file = os.path.join(os.getenv("srcdir"), "vv009-4kfft.cfile")


class test_dtv(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        infile = ts_in_file
        outfile = "vv.cfile"
        testfile = complex_out_file
        file_source = blocks.file_source(
            gr.sizeof_char * 1, infile, False, 0, 0)
        file_source.set_begin_tag(pmt.PMT_NIL)
        bbheader = dtv.dvb_bbheader_bb(
            dtv.STANDARD_DVBT2,
            dtv.FECFRAME_NORMAL,
            dtv.C2_3,
            dtv.RO_0_35,
            dtv.INPUTMODE_NORMAL,
            dtv.INBAND_OFF,
            168,
            4000000)
        bbscrambler = dtv.dvb_bbscrambler_bb(
            dtv.STANDARD_DVBT2,
            dtv.FECFRAME_NORMAL,
            dtv.C2_3)
        bch = dtv.dvb_bch_bb(
            dtv.STANDARD_DVBT2,
            dtv.FECFRAME_NORMAL,
            dtv.C2_3)
        ldpc = dtv.dvb_ldpc_bb(
            dtv.STANDARD_DVBT2,
            dtv.FECFRAME_NORMAL,
            dtv.C2_3,
            dtv.MOD_OTHER)
        interleaver = dtv.dvbt2_interleaver_bb(
            dtv.FECFRAME_NORMAL,
            dtv.C2_3,
            dtv.MOD_64QAM)
        modulator = dtv.dvbt2_modulator_bc(
            dtv.FECFRAME_NORMAL,
            dtv.MOD_64QAM,
            dtv.ROTATION_ON)
        cellinterleaver = dtv.dvbt2_cellinterleaver_cc(
            dtv.FECFRAME_NORMAL,
            dtv.MOD_64QAM,
            3,
            3)
        framemapper = dtv.dvbt2_framemapper_cc(
            dtv.FECFRAME_NORMAL,
            dtv.C2_3,
            dtv.MOD_64QAM,
            dtv.ROTATION_ON,
            3,
            3,
            dtv.CARRIERS_NORMAL,
            dtv.FFTSIZE_4K,
            dtv.GI_1_32,
            dtv.L1_MOD_16QAM,
            dtv.PILOT_PP7,
            2,
            8,
            dtv.PAPR_TR,
            dtv.VERSION_111,
            dtv.PREAMBLE_T2_SISO,
            dtv.INPUTMODE_NORMAL,
            dtv.RESERVED_OFF,
            dtv.L1_SCRAMBLED_OFF,
            dtv.INBAND_OFF)
        freqinterleaver = dtv.dvbt2_freqinterleaver_cc(
            dtv.CARRIERS_NORMAL,
            dtv.FFTSIZE_4K,
            dtv.PILOT_PP7,
            dtv.GI_1_32,
            8,
            dtv.PAPR_TR,
            dtv.VERSION_111,
            dtv.PREAMBLE_T2_SISO
        )
        pilotgenerator = dtv.dvbt2_pilotgenerator_cc(
            dtv.CARRIERS_NORMAL,
            dtv.FFTSIZE_4K,
            dtv.PILOT_PP7,
            dtv.GI_1_32,
            8,
            dtv.PAPR_TR,
            dtv.VERSION_111,
            dtv.PREAMBLE_T2_SISO,
            dtv.MISO_TX1,
            dtv.EQUALIZATION_OFF,
            dtv.BANDWIDTH_8_0_MHZ,
            4096
        )
        paprtr = dtv.dvbt2_paprtr_cc(
            dtv.CARRIERS_NORMAL,
            dtv.FFTSIZE_4K,
            dtv.PILOT_PP7,
            dtv.GI_1_32,
            8,
            dtv.PAPR_TR,
            dtv.VERSION_111,
            3.0,
            10,
            4096
        )
        ofdm_cyclic_prefixer = digital.ofdm_cyclic_prefixer(
            4096,
            4096 + 4096 // 32,
            0,
            '')
        p1insertion = dtv.dvbt2_p1insertion_cc(
            dtv.CARRIERS_NORMAL,
            dtv.FFTSIZE_4K,
            dtv.GI_1_32,
            8,
            dtv.PREAMBLE_T2_SISO,
            dtv.SHOWLEVELS_OFF,
            3.01
        )
        file_sink = blocks.file_sink(gr.sizeof_gr_complex * 1, outfile, False)
        file_sink.set_unbuffered(True)
        self.tb.connect(
            file_source,
            bbheader,
            bbscrambler,
            bch,
            ldpc,
            interleaver,
            modulator,
            cellinterleaver,
            framemapper,
            freqinterleaver,
            pilotgenerator,
            paprtr,
            ofdm_cyclic_prefixer,
            p1insertion,
            file_sink)
        self.tb.run()
        file_sink.close()

        self.assertEqual(getsize(outfile), getsize(testfile))

        out_data = np.fromfile(outfile, dtype=np.float32)
        expected_data = np.fromfile(testfile, dtype=np.float32)
        os.remove(outfile)

        self.assertFloatTuplesAlmostEqual(out_data, expected_data, 5)
        pass


if __name__ == '__main__':
    gr_unittest.run(test_dtv)
