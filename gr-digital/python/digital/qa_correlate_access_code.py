#!/usr/bin/env python
#
# Copyright 2006,2007,2010,2011,2013,2017 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, digital, blocks

default_access_code = '\xAC\xDD\xA4\xE2\xF2\x8C\x20\xFC'

def string_to_1_0_list(s):
    r = []
    for ch in s:
        x = ord(ch)
        for i in range(8):
            t = (x >> i) & 0x1
            r.append(t)

    return r

def to_1_0_string(L):
    return ''.join(map(lambda x: chr(x + ord('0')), L))

class test_correlate_access_code(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        pad = (0,) * 64
        #           0  0  0  1  0  0  0  1
        src_data = (1, 0, 1, 1, 1, 1, 0, 1, 1) + pad + (0,) * 7
        expected_result = pad + (1, 0, 1, 1, 3, 1, 0, 1, 1, 2) + (0,) * 6
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_bb("1011", 0)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)


    def test_002(self):
        code = tuple(string_to_1_0_list(default_access_code))
        access_code = to_1_0_string(code)
        pad = (0,) * 64
        #print code
        #print access_code
        src_data = code + (1, 0, 1, 1) + pad
        expected_result = pad + code + (3, 0, 1, 1)
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_bb(access_code, 0)
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_003(self):
        code = tuple(string_to_1_0_list(default_access_code))
        access_code = to_1_0_string(code)
        pad = (0,) * 64
        #print code
        #print access_code
        src_data = code + (1, 0, 1, 1) + pad
        expected_result = code + (1, 0, 1, 1) + pad
        src = blocks.vector_source_b(src_data)
        op = digital.correlate_access_code_tag_bb(access_code, 0, "test")
        dst = blocks.vector_sink_b()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertEqual(expected_result, result_data)

    def test_004(self):
        code = tuple(string_to_1_0_list(default_access_code))
        access_code = to_1_0_string(code)
        pad = (0,) * 64
        #print code
        #print access_code
        src_bits = code + (1, 0, 1, 1) + pad
        src_data = [2.0*x - 1.0 for x in src_bits]
        expected_result_bits = code + (1, 0, 1, 1) + pad
        expected_result = [2.0*x - 1.0 for x in expected_result_bits]
        src = blocks.vector_source_f(src_data)
        op = digital.correlate_access_code_tag_ff(access_code, 0, "test")
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op, dst)
        self.tb.run()
        result_data = dst.data()
        self.assertFloatTuplesAlmostEqual(expected_result, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_correlate_access_code, "test_correlate_access_code.xml")

