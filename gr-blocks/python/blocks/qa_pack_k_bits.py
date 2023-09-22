#!/usr/bin/env python
#
# Copyright 2006,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import random

from gnuradio import gr, gr_unittest, blocks
import pmt


class test_pack(gr_unittest.TestCase):

    def setUp(self):
        random.seed(0)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [1, 0, 1, 1, 0, 1, 1, 0]
        expected_results = [1, 0, 1, 1, 0, 1, 1, 0]
        src = blocks.vector_source_b(src_data, False)
        op = blocks.pack_k_bits_bb(1)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_002(self):
        src_data = [1, 0, 1, 1, 0, 0, 0, 1]
        expected_results = [2, 3, 0, 1]
        src = blocks.vector_source_b(src_data, False)
        op = blocks.pack_k_bits_bb(2)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        self.assertEqual(expected_results, dst.data())

    def test_003(self):
        src_data = expected_results = [random.randint(0, 3) for x in range(10)]
        src = blocks.vector_source_b(src_data)
        pack = blocks.pack_k_bits_bb(2)
        unpack = blocks.unpack_k_bits_bb(2)
        snk = blocks.vector_sink_b()
        self.tb.connect(src, unpack, pack, snk)
        self.tb.run()
        self.assertEqual(list(expected_results), list(snk.data()))

    def test_004(self):
        # Test tags propagation

        # Tags on the incoming bits
        src_data = [1, 0, 1, 1, 0, 0, 0, 1]
        #src_tag_offsets = [1, 2, 3, 5, 6]
        src_tag_offsets = [1, 2, 3, 5, 6, 7]

        # Ground Truth
        expected_data = [2, 3, 0, 1]
        expected_tag_offsets = [0, 1, 1, 2, 3, 3]

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
        op = blocks.pack_k_bits_bb(2)
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
    gr_unittest.run(test_pack)
