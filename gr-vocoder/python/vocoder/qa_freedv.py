#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, gr_unittest, vocoder, blocks
from gnuradio.vocoder import freedv_api

modes = {
    freedv_api.MODE_1600: "MODE_1600",
    freedv_api.MODE_2400A: "MODE_2400A",
    freedv_api.MODE_2400B: "MODE_2400B",
    freedv_api.MODE_700: "MODE_700",
    freedv_api.MODE_700B: "MODE_700B",
    freedv_api.MODE_700C: "MODE_700C",
    freedv_api.MODE_700D: "MODE_700D",
    freedv_api.MODE_800XA: "MODE_800XA",
}


class test_freedv_vocoder(gr_unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test001_default_instantiation(self):
        enc = vocoder.freedv_tx_ss()
        dec = vocoder.freedv_rx_ss()
        self.assertTrue(enc)
        self.assertTrue(dec)

    def test002_allthemodes(self):
        for idx, (mode, name) in enumerate(modes.items()):
            interleaves = (-1, )
            if mode == freedv_api.MODE_700D:
                interleaves = (1, 4, 16, -1, 0)
            for interleave in interleaves:
                testname = f"test{idx*100+1:03d}_{name}_{interleave}"
                with self.subTest(testname=testname):
                    self.modetest(mode, testname, interleave)

    def modetest(self, mode, testname, interl):
        enc = vocoder.freedv_tx_ss(mode, testname, interleave_frames=interl)
        dec = vocoder.freedv_rx_ss(mode, 0.0, interleave_frames=interl)
        self.assertTrue(
            enc, msg=f"tx mode {mode}{' interleaved' if interl > 1 else ''}")
        self.assertTrue(
            dec, msg=f"rx mode {mode}{' interleaved' if interl > 1 else ''}")
        tb = gr.top_block()
        src = blocks.vector_source_s(list(range(2**15)))
        head = blocks.head(gr.sizeof_short, 10000)
        sink = blocks.vector_sink_s()
        tb.connect(src, enc, dec, head, sink)
        tb.run()
        self.assertTrue(len(sink.data()))
        tb = None
        enc = None
        dec = None


if __name__ == '__main__':
    gr_unittest.run(test_freedv_vocoder)
