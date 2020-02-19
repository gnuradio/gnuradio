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

class test_g721_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = (8,24,36,52,56,64,76,88,104,124,132,148,172,
                196,220,244,280,320,372,416,468,524,580,648)
        src = blocks.vector_source_s(data)
        enc = vocoder.g721_encode_sb()
        dec = vocoder.g721_decode_bs()
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(data, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_g721_vocoder, "test_g721_vocoder.xml")
