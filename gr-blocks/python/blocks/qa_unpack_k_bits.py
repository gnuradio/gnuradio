#!/usr/bin/env python
#
# Copyright 2006,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks

import random
import pmt


class test_unpack(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [1, 0, 1, 1, 0, 1, 1, 0]
        expected_results = [1, 0, 1, 1, 0, 1, 1, 0]
        src = blocks.vector_source_b(src_data, False)
        op = blocks.unpack_k_bits_bb(1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_002(self):
        src_data = [2, 3, 0, 1]
        expected_results = [1, 0, 1, 1, 0, 0, 0, 1]
        src = blocks.vector_source_b(src_data, False)
        op = blocks.unpack_k_bits_bb(2)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_003(self):

        # Tags on the incoming bytes
        src_data = [2, 3, 0, 1]
        src_tag_offsets = [0, 1, 1, 2, 3]

        # Ground Truth
        expected_data = [1, 0, 1, 1, 0, 0, 0, 1]
        expected_tag_offsets = [0, 2, 2, 4, 6]

        test_tags = list()
        tag_indexs = range(len(src_tag_offsets))
        for src_tag in tag_indexs:
            test_tags.append(
                gr.tag_utils.python_to_tag((src_tag_offsets[src_tag],
                                            pmt.intern('tag_byte'),
                                            pmt.from_long(src_tag),
                                            None
                                            ))
            )

        src = blocks.vector_source_b(src_data, False, 1, test_tags)
        op = blocks.unpack_k_bits_bb(2)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()

        # Check the data
        self.assertEqual(expected_data, dst.data())

        # Check the tag values
        self.assertEqual(list(tag_indexs), [
                         pmt.to_python(x.value) for x in dst.tags()])

        # Check the tag offsets
        self.assertEqual(expected_tag_offsets, [x.offset for x in dst.tags()])


if __name__ == '__main__':
    gr_unittest.run(test_unpack)
