#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks, filter, digital

class test_simple_correlator(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_00(self):
        expected_result = (
            0x00, 0x11, 0x22, 0x33,
            0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xaa, 0xbb,
            0xcc, 0xdd, 0xee, 0xff)

        # Filter taps to expand the data to oversample by 8
        # Just using a RRC for some basic filter shape
        taps = filter.firdes.root_raised_cosine(8, 8, 1.0, 0.5, 21)
        
        src = blocks.vector_source_b(expected_result)
        frame = digital.simple_framer(4)
        unpack = blocks.packed_to_unpacked_bb(1, gr.GR_MSB_FIRST)
        expand = filter.interp_fir_filter_fff(8, taps)
        b2f = blocks.char_to_float()
        mult2 = blocks.multiply_const_ff(2)
        sub1 = blocks.add_const_ff(-1)
        op = digital.simple_correlator(4)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, frame, unpack, b2f, mult2, sub1, expand)
        self.tb.connect(expand, op, dst)
        self.tb.run()
        result_data = dst.data()

        self.assertEqual(expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_simple_correlator, "test_simple_correlator.xml")
