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

class test_alaw_vocoder (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block()

    def tearDown (self):
        self.tb = None

    def test001_module_load (self):
        data = (8,24,40,56,72,88,104,120,136,152,168,184,
                200,216,232,248,264,280,296,312,328,344)
        src = blocks.vector_source_s(data)
        enc = vocoder.alaw_encode_sb()
        dec = vocoder.alaw_decode_bs()
        snk = blocks.vector_sink_s()
        self.tb.connect(src, enc, dec, snk)
        self.tb.run()
        actual_result = snk.data()
        self.assertEqual(data, actual_result)


if __name__ == '__main__':
    gr_unittest.run(test_alaw_vocoder, "test_alaw_vocoder.xml")
