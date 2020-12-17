#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, digital, blocks


class test_simple_framer(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_simple_framer_001(self):
        src_data = (0x00, 0x11, 0x22, 0x33,
                    0x44, 0x55, 0x66, 0x77,
                    0x88, 0x99, 0xaa, 0xbb,
                    0xcc, 0xdd, 0xee, 0xff)

        expected_result = [
            0xac,
            0xdd,
            0xa4,
            0xe2,
            0xf2,
            0x8c,
            0x20,
            0xfc,
            0x00,
            0x00,
            0x11,
            0x22,
            0x33,
            0x55,
            0xac,
            0xdd,
            0xa4,
            0xe2,
            0xf2,
            0x8c,
            0x20,
            0xfc,
            0x01,
            0x44,
            0x55,
            0x66,
            0x77,
            0x55,
            0xac,
            0xdd,
            0xa4,
            0xe2,
            0xf2,
            0x8c,
            0x20,
            0xfc,
            0x02,
            0x88,
            0x99,
            0xaa,
            0xbb,
            0x55,
            0xac,
            0xdd,
            0xa4,
            0xe2,
            0xf2,
            0x8c,
            0x20,
            0xfc,
            0x03,
            0xcc,
            0xdd,
            0xee,
            0xff,
            0x55]

        src = blocks.vector_source_b(src_data)
        op = digital.simple_framer(4)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)


if __name__ == '__main__':
    gr_unittest.run(test_simple_framer)
