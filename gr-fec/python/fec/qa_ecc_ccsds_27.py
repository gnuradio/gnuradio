#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
from gnuradio import fec


class test_ccsds_27 (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def xtest_ccsds_27 (self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        expected = (0, 0, 0, 0, 1, 2, 3, 4, 5, 6)
        src = blocks.vector_source_b(src_data)
        enc = fec.encode_ccsds_27_bb()
        b2f = blocks.char_to_float()
        add = blocks.add_const_ff(-0.5)
        mul = blocks.multiply_const_ff(2.0)
        dec = fec.decode_ccsds_27_fb()
        dst = blocks.vector_sink_b()
        self.tb.connect(src, enc, b2f, add, mul, dec, dst)
        self.tb.run()
        dst_data = dst.data()
        self.assertEqual(expected, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_ccsds_27, "test_ccsds_27.xml")
