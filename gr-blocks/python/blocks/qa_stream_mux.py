#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2010,2012,2013,2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import pmt
import os


class test_stream_mux (gr_unittest.TestCase):

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def help_stream_2ff(self, N, stream_sizes):
        v0 = blocks.vector_source_f(N * [1, ], False)
        v1 = blocks.vector_source_f(N * [2, ], False)

        mux = blocks.stream_mux(gr.sizeof_float, stream_sizes)

        dst = blocks.vector_sink_f()

        self.tb.connect(v0, (mux, 0))
        self.tb.connect(v1, (mux, 1))
        self.tb.connect(mux, dst)
        self.tb.run()

        return dst.data()

    def help_stream_ramp_2ff(self, N, stream_sizes):
        r1 = list(range(N))
        r2 = list(range(N))
        r2.reverse()

        v0 = blocks.vector_source_f(r1, False)
        v1 = blocks.vector_source_f(r2, False)

        mux = blocks.stream_mux(gr.sizeof_float, stream_sizes)

        dst = blocks.vector_sink_f()

        self.tb.connect(v0, (mux, 0))
        self.tb.connect(v1, (mux, 1))
        self.tb.connect(mux, dst)
        self.tb.run()

        return dst.data()

    def help_stream_tag_propagation(self, N, stream_sizes):
        src_data1 = stream_sizes[0] * N * [1, ]
        src_data2 = stream_sizes[1] * N * [2, ]
        src_data3 = stream_sizes[2] * N * [3, ]
    # stream_mux scheme (3,2,4)
        src1 = blocks.vector_source_f(src_data1)
        src2 = blocks.vector_source_f(src_data2)
        src3 = blocks.vector_source_f(src_data3)
        tag_stream1 = blocks.stream_to_tagged_stream(gr.sizeof_float, 1,
                                                     stream_sizes[0], 'src1')
        tag_stream2 = blocks.stream_to_tagged_stream(gr.sizeof_float, 1,
                                                     stream_sizes[1], 'src2')
        tag_stream3 = blocks.stream_to_tagged_stream(gr.sizeof_float, 1,
                                                     stream_sizes[2], 'src3')

        mux = blocks.stream_mux(gr.sizeof_float, stream_sizes)
        dst = blocks.vector_sink_f()

        self.tb.connect(src1, tag_stream1)
        self.tb.connect(src2, tag_stream2)
        self.tb.connect(src3, tag_stream3)
        self.tb.connect(tag_stream1, (mux, 0))
        self.tb.connect(tag_stream2, (mux, 1))
        self.tb.connect(tag_stream3, (mux, 2))
        self.tb.connect(mux, dst)
        self.tb.run()

        return (dst.data(), dst.tags())

    def test_stream_2NN_ff(self):
        N = 40
        stream_sizes = [10, 10]
        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]
        self.assertEqual(exp_data, result_data)

    def test_stream_ramp_2NN_ff(self):
        N = 40
        stream_sizes = [10, 10]
        result_data = self.help_stream_ramp_2ff(N, stream_sizes)

        exp_data = [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
                    39.0, 38.0, 37.0, 36.0, 35.0, 34.0, 33.0, 32.0, 31.0, 30.0,
                    10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0,
                    29.0, 28.0, 27.0, 26.0, 25.0, 24.0, 23.0, 22.0, 21.0, 20.0,
                    20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0,
                    19.0, 18.0, 17.0, 16.0, 15.0, 14.0, 13.0, 12.0, 11.0, 10.0,
                    30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0,
                    9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.0]
        self.assertEqual(exp_data, result_data)

    def test_stream_2NM_ff(self):
        N = 40
        stream_sizes = [7, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0]

        self.assertEqual(exp_data, result_data)

    def test_stream_2MN_ff(self):
        N = 37
        stream_sizes = [7, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    2.0]

        self.assertEqual(exp_data, result_data)

    def test_stream_2N0_ff(self):
        N = 30
        stream_sizes = [7, 0]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                    1.0, 1.0]

        self.assertEqual(exp_data, result_data)

    def test_stream_20N_ff(self):
        N = 30
        stream_sizes = [0, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data = [2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                    2.0, 2.0, 2.0]

        self.assertEqual(exp_data, result_data)

    def test_largeN_ff(self):
        stream_sizes = [3, 8191]
        r1 = [1, ] * stream_sizes[0]
        r2 = [2, ] * stream_sizes[1]
        v0 = blocks.vector_source_f(r1, repeat=False)
        v1 = blocks.vector_source_f(r2, repeat=False)
        mux = blocks.stream_mux(gr.sizeof_float, stream_sizes)
        dst = blocks.vector_sink_f()
        self.tb.connect(v0, (mux, 0))
        self.tb.connect(v1, (mux, 1))
        self.tb.connect(mux, dst)
        self.tb.run()
        self.assertEqual(r1 + r2, dst.data())

    def test_tag_propagation(self):
        N = 10  # Block length
        stream_sizes = [1, 2, 3]

        expected_result = N * (stream_sizes[0] * [1, ] +
                               stream_sizes[1] * [2, ] +
                               stream_sizes[2] * [3, ])
        # check the data
        (result, tags) = self.help_stream_tag_propagation(N, stream_sizes)
        self.assertFloatTuplesAlmostEqual(expected_result, result, places=6)

        # check the tags
        expected_tag_offsets_src1 = [sum(stream_sizes) * i for i in range(N)]
        expected_tag_offsets_src2 = [stream_sizes[0] +
                                     sum(stream_sizes) * i for i in range(N)]
        expected_tag_offsets_src3 = [stream_sizes[0] + stream_sizes[1] +
                                     sum(stream_sizes) * i for i in range(N)]
        tags_src1 = [
            tag for tag in tags if pmt.eq(
                tag.key, pmt.intern('src1'))]
        tags_src2 = [
            tag for tag in tags if pmt.eq(
                tag.key, pmt.intern('src2'))]
        tags_src3 = [
            tag for tag in tags if pmt.eq(
                tag.key, pmt.intern('src3'))]

        for i in range(len(expected_tag_offsets_src1)):
            self.assertEqual(expected_tag_offsets_src1[i], tags_src1[i].offset)
        for i in range(len(expected_tag_offsets_src2)):
            self.assertEqual(expected_tag_offsets_src2[i], tags_src2[i].offset)
        for i in range(len(expected_tag_offsets_src3)):
            self.assertEqual(expected_tag_offsets_src3[i], tags_src3[i].offset)


if __name__ == '__main__':
    gr_unittest.run(test_stream_mux)
