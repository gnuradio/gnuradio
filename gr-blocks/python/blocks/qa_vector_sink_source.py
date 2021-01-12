#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks
import pmt
import math


def make_tag(key, value, offset, srcid=None):
    tag = gr.tag_t()
    tag.key = pmt.string_to_symbol(key)
    tag.value = pmt.to_pmt(value)
    tag.offset = offset
    if srcid is not None:
        tag.srcid = pmt.to_pmt(srcid)
    return tag


def compare_tags(a, b):
    return a.offset == b.offset and pmt.equal(a.key, b.key) and \
        pmt.equal(a.value, b.value) and pmt.equal(a.srcid, b.srcid)


class test_vector_sink_source(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        # Test that sink has data set in source for the simplest case
        src_data = [float(x) for x in range(16)]
        expected_result = src_data

        src = blocks.vector_source_f(src_data)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_002(self):
        # Test vectors (the gnuradio vector I/O type)
        src_data = [float(x) for x in range(16)]
        expected_result = src_data

        src = blocks.vector_source_f(src_data, False, 2)
        dst = blocks.vector_sink_f(2)

        self.tb.connect(src, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_003(self):
        # Test that we can only make vectors (the I/O type) if the input
        # vector has sufficient size
        src_data = [float(x) for x in range(16)]
        self.assertRaises(
            ValueError, lambda: blocks.vector_source_f(
                src_data, False, 3))

    def test_004(self):
        # Test sending and receiving tagged streams
        src_data = [float(x) for x in range(16)]
        expected_result = src_data
        src_tags = [make_tag('key', 'val', 0, 'src')]
        expected_tags = src_tags[:]

        src = blocks.vector_source_f(src_data, repeat=False, tags=src_tags)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(expected_result, result_data)
        self.assertEqual(len(result_tags), 1)
        self.assertTrue(compare_tags(expected_tags[0], result_tags[0]))

    def test_005(self):
        # Test that repeat works (with tagged streams)
        length = 16
        src_data = [float(x) for x in range(length)]
        expected_result = src_data + src_data
        src_tags = [make_tag('key', 'val', 0, 'src')]
        expected_tags = [make_tag('key', 'val', 0, 'src'),
                         make_tag('key', 'val', length, 'src')]

        src = blocks.vector_source_f(src_data, repeat=True, tags=src_tags)
        head = blocks.head(gr.sizeof_float, 2 * length)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, head, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(expected_result, result_data)
        self.assertEqual(len(result_tags), 2)
        self.assertTrue(compare_tags(expected_tags[0], result_tags[0]))
        self.assertTrue(compare_tags(expected_tags[1], result_tags[1]))

    def test_006(self):
        # Test set_data
        src_data = [float(x) for x in range(16)]
        expected_result = src_data

        src = blocks.vector_source_f((3, 1, 4))
        dst = blocks.vector_sink_f()
        src.set_data(src_data)

        self.tb.connect(src, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_007(self):
        # Test set_repeat
        src_data = [float(x) for x in range(16)]
        expected_result = src_data

        src = blocks.vector_source_f(src_data, True)
        dst = blocks.vector_sink_f()
        src.set_repeat(False)

        self.tb.connect(src, dst)
        # will timeout if set_repeat does not work
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)


if __name__ == '__main__':
    gr_unittest.run(test_vector_sink_source)
