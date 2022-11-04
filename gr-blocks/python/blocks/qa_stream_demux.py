#!/usr/bin/env python
#
# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#
#


from gnuradio import gr, gr_unittest, blocks
import pmt
import os


class qa_stream_demux(gr_unittest.TestCase):

    def setUp(self):
        os.environ['GR_CONF_CONTROLPORT_ON'] = 'False'
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def help_stream_2ff(self, N, stream_sizes):
        v = blocks.vector_source_f(N * [1, ] + N * [2, ], False)

        demux = blocks.stream_demux(gr.sizeof_float, stream_sizes)

        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()

        self.tb.connect(v, demux)
        self.tb.connect((demux, 0), dst0)
        self.tb.connect((demux, 1), dst1)
        self.tb.run()

        return (dst0.data(), dst1.data())

    def help_stream_ramp_2ff(self, N, stream_sizes):
        r = list(range(N)) + list(reversed(range(N)))

        v = blocks.vector_source_f(r, False)

        demux = blocks.stream_demux(gr.sizeof_float, stream_sizes)

        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()

        self.tb.connect(v, demux)
        self.tb.connect((demux, 0), dst0)
        self.tb.connect((demux, 1), dst1)
        self.tb.run()

        return (dst0.data(), dst1.data())

    def help_stream_tag_propagation(self, N, stream_sizes):
        src_data = (stream_sizes[0] * [1, ] + stream_sizes[1] *
                    [2, ] + stream_sizes[2] * [3, ]) * N

        src = blocks.vector_source_f(src_data, False)

        tag_stream1 = blocks.stream_to_tagged_stream(gr.sizeof_float, 1,
                                                     stream_sizes[0], 'src1')
        tag_stream2 = blocks.stream_to_tagged_stream(gr.sizeof_float, 1,
                                                     stream_sizes[1], 'src2')
        tag_stream3 = blocks.stream_to_tagged_stream(gr.sizeof_float, 1,
                                                     stream_sizes[2], 'src3')

        demux = blocks.stream_demux(gr.sizeof_float, stream_sizes)
        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()
        dst2 = blocks.vector_sink_f()

        self.tb.connect(src, tag_stream1)
        self.tb.connect(tag_stream1, tag_stream2)
        self.tb.connect(tag_stream2, tag_stream3)
        self.tb.connect(tag_stream3, demux)
        self.tb.connect((demux, 0), dst0)
        self.tb.connect((demux, 1), dst1)
        self.tb.connect((demux, 2), dst2)
        self.tb.run()

        return (dst0, dst1, dst2)

    def test_stream_2NN_ff(self):
        N = 40
        stream_sizes = [10, 10]
        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data0 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]
        exp_data1 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]

        self.assertEqual(exp_data0, result_data[0])
        self.assertEqual(exp_data1, result_data[1])

    def test_stream_ramp_2NN_ff(self):
        N = 40
        stream_sizes = [10, 10]
        result_data = self.help_stream_ramp_2ff(N, stream_sizes)

        exp_data0 = [
            0.0,
            1.0,
            2.0,
            3.0,
            4.0,
            5.0,
            6.0,
            7.0,
            8.0,
            9.0,
            20.0,
            21.0,
            22.0,
            23.0,
            24.0,
            25.0,
            26.0,
            27.0,
            28.0,
            29.0,
            39.0,
            38.0,
            37.0,
            36.0,
            35.0,
            34.0,
            33.0,
            32.0,
            31.0,
            30.0,
            19.0,
            18.0,
            17.0,
            16.0,
            15.0,
            14.0,
            13.0,
            12.0,
            11.0,
            10.0]
        exp_data1 = [
            10.0,
            11.0,
            12.0,
            13.0,
            14.0,
            15.0,
            16.0,
            17.0,
            18.0,
            19.0,
            30.0,
            31.0,
            32.0,
            33.0,
            34.0,
            35.0,
            36.0,
            37.0,
            38.0,
            39.0,
            29.0,
            28.0,
            27.0,
            26.0,
            25.0,
            24.0,
            23.0,
            22.0,
            21.0,
            20.0,
            9.0,
            8.0,
            7.0,
            6.0,
            5.0,
            4.0,
            3.0,
            2.0,
            1.0,
            0.0]

        self.assertEqual(exp_data0, result_data[0])
        self.assertEqual(exp_data1, result_data[1])

    def test_stream_2NM_ff(self):
        N = 40
        stream_sizes = [7, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data0 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]

        exp_data1 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]

        self.assertEqual(exp_data0, result_data[0])
        self.assertEqual(exp_data1, result_data[1])

    def test_stream_2MN_ff(self):
        N = 37
        stream_sizes = [7, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data0 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0]

        exp_data1 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0]

        self.assertEqual(exp_data0, result_data[0])
        self.assertEqual(exp_data1, result_data[1])

    def test_stream_2N0_ff(self):
        N = 30
        stream_sizes = [7, 0]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data0 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0]
        exp_data1 = []

        self.assertEqual(exp_data0, result_data[0])
        self.assertEqual(exp_data1, result_data[1])

    def test_stream_20N_ff(self):
        N = 30
        stream_sizes = [0, 9]
        self.help_stream_2ff(N, stream_sizes)

        result_data = self.help_stream_2ff(N, stream_sizes)

        exp_data0 = []
        exp_data1 = [1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                     1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,
                     2.0, 2.0, 2.0, 2.0, 2.0, 2.0]

        self.assertEqual(exp_data0, result_data[0])
        self.assertEqual(exp_data1, result_data[1])

    def test_largeN_ff(self):
        stream_sizes = [3, 8191]
        r0 = [1.0, ] * stream_sizes[0]
        r1 = [2.0, ] * stream_sizes[1]
        v = blocks.vector_source_f(r0 + r1, repeat=False)
        demux = blocks.stream_demux(gr.sizeof_float, stream_sizes)
        dst0 = blocks.vector_sink_f()
        dst1 = blocks.vector_sink_f()
        self.tb.connect(v, demux)
        self.tb.connect((demux, 0), dst0)
        self.tb.connect((demux, 1), dst1)
        self.tb.run()
        self.assertEqual(r0, dst0.data())
        self.assertEqual(r1, dst1.data())

    def test_tag_propagation(self):
        N = 10  # Block length
        stream_sizes = [1, 2, 3]

        expected_result0 = N * (stream_sizes[0] * [1, ])
        expected_result1 = N * (stream_sizes[1] * [2, ])
        expected_result2 = N * (stream_sizes[2] * [3, ])

        # check the data
        (result0, result1, result2) = self.help_stream_tag_propagation(N, stream_sizes)
        self.assertFloatTuplesAlmostEqual(
            expected_result0, result0.data(), places=6)
        self.assertFloatTuplesAlmostEqual(
            expected_result1, result1.data(), places=6)
        self.assertFloatTuplesAlmostEqual(
            expected_result2, result2.data(), places=6)

        # check the tags - result0
        tags = result0.tags()
        expected_tag_offsets_src1 = list(
            range(0, stream_sizes[0] * N, stream_sizes[0]))
        expected_tag_offsets_src2 = list(
            range(0, stream_sizes[0] * N, stream_sizes[0]))
        expected_tag_offsets_src3 = list(
            range(0, stream_sizes[0] * N, stream_sizes[0]))
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

        # check the tags - result1
        tags = result1.tags()
        expected_tag_offsets_src1 = list(
            range(0, stream_sizes[1] * N, stream_sizes[0]))
        expected_tag_offsets_src2 = list(
            range(1, stream_sizes[1] * N, stream_sizes[1]))
        expected_tag_offsets_src3 = list()
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

        # check the tags - result2
        tags = result2.tags()
        expected_tag_offsets_src1 = list(
            range(0, stream_sizes[2] * N, stream_sizes[0]))
        expected_tag_offsets_src2 = list(
            range(1, stream_sizes[2] * N, stream_sizes[2]))
        expected_tag_offsets_src3 = list(
            range(0, stream_sizes[2] * N, stream_sizes[2]))
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
    gr_unittest.run(qa_stream_demux)
