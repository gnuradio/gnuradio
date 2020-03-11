#!/usr/bin/env python
#
# Copyright 2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, vocoder, blocks

class test_ulaw_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = (8,24,40,56,72,88,104,120,132,148,164,180,
                196,212,228,244,260,276,292,308,324,340)
        src = blocks.vector_source_s(data)
        enc = vocoder.ulaw_encode_sb()
        dec = vocoder.ulaw_decode_bs()
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(data, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_ulaw_vocoder, "test_ulaw_vocoder.xml")
