#!/usr/bin/env python
#
# Copyright 2012,2013 Free Software Foundation, Inc.
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

import pmt
from gnuradio import gr, gr_unittest, digital, blocks

class test_chunks_to_symbols(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_bc_001(self):
        const = [ 1+0j, 0+1j,
                 -1+0j, 0-1j]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (1+0j, 0+1j, -1+0j, 0-1j,
                           0-1j, -1+0j, 0+1j, 1+0j)

        src = blocks.vector_source_b(src_data)
        op = digital.chunks_to_symbols_bc(const)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_bf_002(self):
        const = [-3, -1, 1, 3]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (-3, -1, 1, 3,
                            3, 1, -1, -3)

        src = blocks.vector_source_b(src_data)
        op = digital.chunks_to_symbols_bf(const)

        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_ic_003(self):
        const = [ 1+0j, 0+1j,
                 -1+0j, 0-1j]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (1+0j, 0+1j, -1+0j, 0-1j,
                           0-1j, -1+0j, 0+1j, 1+0j)

        src = blocks.vector_source_i(src_data)
        op = digital.chunks_to_symbols_ic(const)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_if_004(self):
        const = [-3, -1, 1, 3]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (-3, -1, 1, 3,
                            3, 1, -1, -3)

        src = blocks.vector_source_i(src_data)
        op = digital.chunks_to_symbols_if(const)

        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_sc_005(self):
        const = [ 1+0j, 0+1j,
                 -1+0j, 0-1j]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (1+0j, 0+1j, -1+0j, 0-1j,
                           0-1j, -1+0j, 0+1j, 1+0j)

        src = blocks.vector_source_s(src_data)
        op = digital.chunks_to_symbols_sc(const)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_sf_006(self):
        const = [-3, -1, 1, 3]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (-3, -1, 1, 3,
                            3, 1, -1, -3)

        src = blocks.vector_source_s(src_data)
        op = digital.chunks_to_symbols_sf(const)

        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)


    def test_sf_callback(self):
	constA = [-3, -1, 1, 3]
        constB = [12, -12, 6, -6]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result=(12, -12, 6, -6, -6, 6, -12, 12)

	src = blocks.vector_source_s(src_data, False, 1, "")
        op = digital.chunks_to_symbols_sf(constA)
        op.set_symbol_table(constB)
        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_sc_callback(self):
        constA = [-3.0+1j, -1.0-1j, 1.0+1j, 3-1j]
        constB = [12.0+1j, -12.0-1j, 6.0+1j, -6-1j]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result=(12.0+1j, -12.0-1j, 6.0+1j, -6-1j, -6-1j, 6+1j, -12-1j, 12+1j)

	src = blocks.vector_source_s(src_data, False, 1, "")
        op = digital.chunks_to_symbols_sc(constA)
        op.set_symbol_table(constB)
        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()
        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_sf_tag(self):
        constA = [-3.0, -1.0, 1.0, 3]
        constB = [12.0, -12.0, 6.0, -6]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (-3, -1, 1, 3,
                            -6, 6, -12, 12)
        first_tag = gr.tag_t()
        first_tag.key = pmt.intern("set_symbol_table")
        first_tag.value = pmt.init_f32vector(len(constA), constA)
        first_tag.offset = 0
        second_tag = gr.tag_t()
        second_tag.key = pmt.intern("set_symbol_table")
        second_tag.value = pmt.init_f32vector(len(constB), constB)
        second_tag.offset = 4

        src = blocks.vector_source_s(src_data, False, 1, [first_tag, second_tag])
        op = digital.chunks_to_symbols_sf(constB)

        dst = blocks.vector_sink_f()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

    def test_sc_tag(self):
        constA = [-3.0+1j, -1.0-1j, 1.0+1j, 3-1j]
        constB = [12.0+1j, -12.0-1j, 6.0+1j, -6-1j]
        src_data = (0, 1, 2, 3, 3, 2, 1, 0)
        expected_result = (-3+1j, -1-1j, 1+1j, 3-1j,
                            -6-1j, 6+1j, -12-1j, 12+1j)
        first_tag = gr.tag_t()
        first_tag.key = pmt.intern("set_symbol_table")
        first_tag.value = pmt.init_c32vector(len(constA), constA)
        first_tag.offset = 0
        second_tag = gr.tag_t()
        second_tag.key = pmt.intern("set_symbol_table")
        second_tag.value = pmt.init_c32vector(len(constB), constB)
        second_tag.offset = 4

        src = blocks.vector_source_s(src_data, False, 1, [first_tag, second_tag])
        op = digital.chunks_to_symbols_sc(constB)

        dst = blocks.vector_sink_c()
        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        actual_result = dst.data()
        self.assertEqual(expected_result, actual_result)

if __name__ == '__main__':
    gr_unittest.run(test_chunks_to_symbols, "test_chunks_to_symbols.xml")
