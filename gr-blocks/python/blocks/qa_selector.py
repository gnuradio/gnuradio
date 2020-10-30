#!/usr/bin/env python
#
# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_selector(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_select_same(self):
        src_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        expected_result = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        expected_drop = ()

        num_inputs = 4
        num_outputs = 4
        input_index = 1
        output_index = 2

        op = blocks.selector(gr.sizeof_char, input_index, output_index)

        src = []
        dst = []
        for ii in range(num_inputs):
            src.append(blocks.vector_source_b(src_data))
            self.tb.connect(src[ii], (op, ii))
        for jj in range(num_outputs):
            dst.append(blocks.vector_sink_b())
            self.tb.connect((op, jj), dst[jj])

        self.tb.run()

        dst_data = dst[output_index].data()

        self.assertEqual(expected_result, dst_data)

    def test_select_input(self):

        num_inputs = 4
        num_outputs = 4
        input_index = 1
        output_index = 2

        op = blocks.selector(gr.sizeof_char, input_index, output_index)

        src = []
        dst = []
        for ii in range(num_inputs):
            src_data = [ii + 1] * 10
            src.append(blocks.vector_source_b(src_data))
            self.tb.connect(src[ii], (op, ii))
        for jj in range(num_outputs):
            dst.append(blocks.vector_sink_b())
            self.tb.connect((op, jj), dst[jj])

        self.tb.run()

        expected_result = [input_index + 1] * 10
        dst_data = list(dst[output_index].data())

        self.assertEqual(expected_result, dst_data)

    def test_dump(self):

        num_inputs = 4
        num_outputs = 4
        input_index = 1
        output_index = 2
        output_not_selected = 3

        op = blocks.selector(gr.sizeof_char, input_index, output_index)

        src = []
        dst = []
        for ii in range(num_inputs):
            src_data = [ii + 1] * 10
            src.append(blocks.vector_source_b(src_data))
            self.tb.connect(src[ii], (op, ii))
        for jj in range(num_outputs):
            dst.append(blocks.vector_sink_b())
            self.tb.connect((op, jj), dst[jj])

        self.tb.run()

        expected_result = []
        dst_data = list(dst[output_not_selected].data())

        self.assertEqual(expected_result, dst_data)

    def test_not_enabled(self):
        src_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
        expected_result = []

        num_inputs = 4
        num_outputs = 4
        input_index = 1
        output_index = 2

        op = blocks.selector(gr.sizeof_char, input_index, output_index)
        op.set_enabled(False)

        src = []
        dst = []
        for ii in range(num_inputs):
            src.append(blocks.vector_source_b(src_data))
            self.tb.connect(src[ii], (op, ii))
        for jj in range(num_outputs):
            dst.append(blocks.vector_sink_b())
            self.tb.connect((op, jj), dst[jj])

        self.tb.run()

        dst_data = dst[output_index].data()
        self.assertEqual(expected_result, dst_data)

    # These tests cannot be run as set_index can only be called after check_topology is called
    # def test_set_indices(self):

    #     num_inputs = 4; num_outputs = 4
    #     input_index = 1; output_index = 2

    #     op = blocks.selector(gr.sizeof_char, 0, 0)

    #     src = []
    #     dst = []
    #     for ii in range(num_inputs):
    #         src_data = [ii+1]*10
    #         src.append( blocks.vector_source_b(src_data))
    #         self.tb.connect(src[ii], (op,ii))
    #     for jj in range(num_outputs):
    #         dst.append(blocks.vector_sink_b())
    #         self.tb.connect((op,jj),dst[jj])

    #     op.set_input_index(input_index)
    #     op.set_output_index(output_index)

    #     self.tb.run()

    #     expected_result = [input_index+1]*10
    #     dst_data = list(dst[output_index].data())

    #     self.assertEqual(expected_result, dst_data)

    # def test_dont_set_indices(self):

    #     num_inputs = 4; num_outputs = 4
    #     input_index = 1; output_index = 2

    #     op = blocks.selector(gr.sizeof_char, 0, 0)
    #     #op.set_input_index(input_index)
    #     #op.set_output_index(output_index)

    #     src = []
    #     dst = []
    #     for ii in range(num_inputs):
    #         src_data = [ii+1]*10
    #         src.append( blocks.vector_source_b(src_data))
    #         self.tb.connect(src[ii], (op,ii))
    #     for jj in range(num_outputs):
    #         dst.append(blocks.vector_sink_b())
    #         self.tb.connect((op,jj),dst[jj])

    #     self.tb.run()

    #     expected_result = [input_index+1]*10
    #     dst_data = list(dst[output_index].data())

    #     self.assertNotEqual(expected_result, dst_data)

    def test_float_vector(self):

        num_inputs = 4
        num_outputs = 4
        input_index = 1
        output_index = 2

        veclen = 3

        op = blocks.selector(
            gr.sizeof_float * veclen,
            input_index,
            output_index)

        src = []
        dst = []
        for ii in range(num_inputs):
            src_data = [float(ii) + 1] * 10 * veclen
            src.append(
                blocks.vector_source_f(
                    src_data,
                    repeat=False,
                    vlen=veclen))
            self.tb.connect(src[ii], (op, ii))
        for jj in range(num_outputs):
            dst.append(blocks.vector_sink_f(vlen=veclen))
            self.tb.connect((op, jj), dst[jj])

        self.tb.run()

        expected_result = [float(input_index) + 1] * 10 * veclen
        dst_data = list(dst[output_index].data())

        self.assertEqual(expected_result, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_selector)
