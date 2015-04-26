#!/usr/bin/env python
#
# Copyright 2008,2010,2013 Free Software Foundation, Inc.
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
        src_data = [float(x) for x in range(16)]
        expected_result = tuple(src_data)

        src = blocks.vector_source_f(src_data)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_002(self):
        src_data = [float(x) for x in range(16)]
        expected_result = tuple(src_data)

        src = blocks.vector_source_f(src_data, False, 2)
        dst = blocks.vector_sink_f(2)

        self.tb.connect(src, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_003(self):
        src_data = [float(x) for x in range(16)]
        expected_result = tuple(src_data)
        self.assertRaises(RuntimeError, lambda : blocks.vector_source_f(src_data, False, 3))

    def test_004(self):
        src_data = [float(x) for x in range(16)]
        expected_result = tuple(src_data)
        src_tags = tuple([make_tag('key', 'val', 0, 'src')])
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
        length = 16
        src_data = [float(x) for x in range(length)]
        expected_result = tuple(src_data + src_data)
        src_tags = tuple([make_tag('key', 'val', 0, 'src')])
        expected_tags = tuple([make_tag('key', 'val', 0, 'src'),
                               make_tag('key', 'val', length, 'src')])

        src = blocks.vector_source_f(src_data, repeat=True, tags=src_tags)
        head = blocks.head(gr.sizeof_float, 2*length)
        dst = blocks.vector_sink_f()

        self.tb.connect(src, head, dst)
        self.tb.run()
        result_data = dst.data()
        result_tags = dst.tags()
        self.assertEqual(expected_result, result_data)
        self.assertEqual(len(result_tags), 2)
        self.assertTrue(compare_tags(expected_tags[0], result_tags[0]))
        self.assertTrue(compare_tags(expected_tags[1], result_tags[1]))

if __name__ == '__main__':
    gr_unittest.run(test_vector_sink_source, "test_vector_sink_source.xml")

