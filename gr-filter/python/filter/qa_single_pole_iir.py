#!/usr/bin/env python
#
# Copyright 2005,2007,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, filter, blocks


class test_single_pole_iir_filter(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_ff_001(self):
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = src_data
        src = blocks.vector_source_f(src_data)
        op = filter.single_pole_iir_filter_ff(1.0)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data)

    def test_ff_002(self):
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = (
            0,
            125,
            359.375,
            689.453125,
            1103.271484,
            1590.36255)
        src = blocks.vector_source_f(src_data)
        op = filter.single_pole_iir_filter_ff(0.125)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 3)

    def test_ff_003(self):
        block_size = 2
        src_data = (0, 1000, 2000, 3000, 4000, 5000)
        expected_result = (0, 125, 250, 484.375, 718.75, 1048.828125)
        src = blocks.vector_source_f(src_data)
        s2p = blocks.stream_to_vector(gr.sizeof_float, block_size)
        op = filter.single_pole_iir_filter_ff(0.125, block_size)
        p2s = blocks.vector_to_stream(gr.sizeof_float, block_size)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, s2p, op, p2s, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 3)

    def test_ff_quick_start_step(self):
        alpha = 0.125
        src_data = (2.0,) * 8
        src = blocks.vector_source_f(src_data)
        quick_start = filter.single_pole_iir_filter_ff(alpha, quick_start=True)
        fixed_alpha = filter.single_pole_iir_filter_ff(alpha)
        quick_dst = blocks.vector_sink_f()
        fixed_dst = blocks.vector_sink_f()
        self.tb.connect(src, quick_start, quick_dst)
        self.tb.connect(src, fixed_alpha, fixed_dst)
        self.tb.run()
        self.assertFloatTuplesAlmostEqual(src_data, quick_dst.data())
        expected_fixed = tuple(2.0 * (1.0 - (1.0 - alpha)**n)
                               for n in range(1, len(src_data) + 1))
        self.assertFloatTuplesAlmostEqual(expected_fixed, fixed_dst.data())

    def test_ff_quick_start_transition(self):
        src = blocks.vector_source_f((8.0, 0.0, 4.0, 4.0, 8.0, 0.0))
        op = filter.single_pole_iir_filter_ff(0.25, quick_start=True)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        # The first four outputs are running means, followed by fixed-alpha smoothing.
        self.assertFloatTuplesAlmostEqual((8.0, 4.0, 4.0, 4.0, 5.0, 3.75),
                                          dst.data())

    def test_cc_001(self):
        src_data = (
            0 + 0j,
            1000 + 1000j,
            2000 + 2000j,
            3000 + 3000j,
            4000 + 4000j,
            5000 + 5000j)
        expected_result = src_data
        src = blocks.vector_source_c(src_data)
        op = filter.single_pole_iir_filter_cc(1.0)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data)

    def test_cc_002(self):
        src_data = (complex(0, 0), complex(1000, -
                                           1000), complex(2000, -
                                                          2000), complex(3000, -
                                                                         3000), complex(4000, -
                                                                                        4000), complex(5000, -
                                                                                                       5000))
        expected_result = (complex(0, 0), complex(125, -
                                                  125), complex(359.375, -
                                                                359.375), complex(689.453125, -
                                                                                  689.453125), complex(1103.271484, -
                                                                                                       1103.271484), complex(1590.36255, -
                                                                                                                             1590.36255))
        src = blocks.vector_source_c(src_data)
        op = filter.single_pole_iir_filter_cc(0.125)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 3)

    def test_cc_003(self):
        block_size = 2
        src_data = (complex(0, 0), complex(1000, -
                                           1000), complex(2000, -
                                                          2000), complex(3000, -
                                                                         3000), complex(4000, -
                                                                                        4000), complex(5000, -
                                                                                                       5000))
        expected_result = (complex(0, 0), complex(125, -
                                                  125), complex(250, -
                                                                250), complex(484.375, -
                                                                              484.375), complex(718.75, -
                                                                                                718.75), complex(1048.828125, -
                                                                                                                 1048.828125))
        src = blocks.vector_source_c(src_data)
        s2p = blocks.stream_to_vector(gr.sizeof_gr_complex, block_size)
        op = filter.single_pole_iir_filter_cc(0.125, block_size)
        p2s = blocks.vector_to_stream(gr.sizeof_gr_complex, block_size)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, s2p, op, p2s, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 3)

    def test_cc_quick_start_vectors(self):
        vlen = 2
        src_data = (2 + 4j, -8 + 0j, 6 - 4j, 4j, -2 + 2j, 4 + 6j, 10 + 0j, -2j)
        expected = (2 + 4j, -8 + 0j, 4 + 0j, -4 + 2j, 1 + 1j, 4j, 5.5 + 0.5j, 1j)
        src = blocks.vector_source_c(src_data, False, vlen)
        op = filter.single_pole_iir_filter_cc(0.5, vlen, quick_start=True)
        dst = blocks.vector_sink_c(vlen)
        self.tb.connect(src, op, dst)
        self.tb.run(max_noutput_items=1)
        self.assertComplexTuplesAlmostEqual(expected, dst.data())


if __name__ == '__main__':
    gr_unittest.run(test_single_pole_iir_filter)
