#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest
import qa_basic_flow_graph


def all_counts ():
    return (gr.block_ncurrently_allocated (),
            gr.block_detail_ncurrently_allocated (),
            gr.buffer_ncurrently_allocated (),
            gr.buffer_reader_ncurrently_allocated ())


class wrap_add(gr.hier_block):
    def __init__(self, fg, a):
        add = gr.add_const_ii (a)
        gr.hier_block.__init__ (self, fg, add, add)

class mult_add(gr.hier_block):
    def __init__(self, fg, m, a):
        mult = gr.multiply_const_ii (m)
        add = gr.add_const_ii (a)
        fg.connect (mult, add)
        gr.hier_block.__init__ (self, fg, mult, add)


class test_flow_graph (qa_basic_flow_graph.test_basic_flow_graph):

    def setUp (self):
        ''' override qa_basic_flow_graph setUp in order to use our class'''
        self.fg = gr.flow_graph ()

    def tearDown (self):
        qa_basic_flow_graph.test_basic_flow_graph.tearDown (self)


    # we inherit all their tests, so we can be sure we don't break
    # any of the underlying code


    def leak_check (self, fct):
        begin = all_counts ()
        fct ()
        # tear down early so we can check for leaks
        self.tearDown ()
        end = all_counts ()
        self.assertEqual (begin, end)

    def test_100_tsort_null (self):
        self.assertEqual ([], self.fg.topological_sort (self.fg.all_blocks ()))

    def test_101_tsort_two (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect ((src1, 0), (dst1, 0))
        fg.validate ()
        self.assertEqual ([src1, dst1], fg.topological_sort (fg.all_blocks ()))
        
    def test_102_tsort_three_a (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (src1, nop1)
        fg.connect (nop1, dst1)
        fg.validate ()
        self.assertEqual ([src1, nop1, dst1], fg.topological_sort (fg.all_blocks ()))
        
    def test_103_tsort_three_b (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (nop1, dst1)
        fg.connect (src1, nop1)
        fg.validate ()
        self.assertEqual ([src1, nop1, dst1], fg.topological_sort (fg.all_blocks ()))
        
    def test_104_trivial_dag_check (self):
        fg = self.fg
        nop1 = gr.nop (gr.sizeof_int)
        fg.connect (nop1, nop1)
        fg.validate ()
        self.assertRaises (gr.NotDAG,
                           lambda : fg.topological_sort (fg.all_blocks ()))

    def test_105 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        src2 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        nop3 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        
        fg.connect (src1, nop1)
        fg.connect (src2, nop2)
        fg.connect (nop1, (nop3, 0))
        fg.connect (nop2, (nop3, 1))
        fg.connect (nop3, dst1)
        fg.validate ()
        ts = fg.topological_sort (fg.all_blocks ())
        self.assertEqual ([src2, nop2, src1, nop1, nop3, dst1], ts)
                     

    def test_106 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        src2 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        nop3 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        
        fg.connect (nop3, dst1)
        fg.connect (nop2, (nop3, 1))
        fg.connect (nop1, (nop3, 0))
        fg.connect (src2, nop2)
        fg.connect (src1, nop1)
        fg.validate ()
        ts = fg.topological_sort (fg.all_blocks ())
        self.assertEqual ([src2, nop2, src1, nop1, nop3, dst1], ts)

    def test_107 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        src2 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        
        fg.connect (src1, nop1)
        fg.connect (nop1, dst1)
        fg.connect (src2, nop2)
        fg.connect (nop2, dst2)
        fg.validate ()
        ts = fg.topological_sort (fg.all_blocks ())
        self.assertEqual ([src2, nop2, dst2, src1, nop1, dst1], ts)

    def test_108 (self):
        self.leak_check (self.body_108)
        
    def body_108 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        src2 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        
        fg.connect (nop2, dst2)
        fg.connect (src1, nop1)
        fg.connect (src2, nop2)
        fg.connect (nop1, dst1)
        fg.validate ()
        ts = fg.topological_sort (fg.all_blocks ())
        self.assertEqual ([src2, nop2, dst2, src1, nop1, dst1], ts)
        self.assertEqual ((6,0,0,0), all_counts ())

    def test_109__setup_connections (self):
        self.leak_check (self.body_109)

    def body_109 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (src1, dst1)
        fg._setup_connections ()
        self.assertEqual ((2,2,1,1), all_counts ())

    def test_110_scheduler (self):
        self.leak_check (self.body_110)

    def body_110 (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        src1 = gr.vector_source_i (src_data)
        dst1 = gr.vector_sink_i ()
        fg.connect ((src1, 0), (dst1, 0))
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (src_data, dst_data)

    def test_111_scheduler (self):
        self.leak_check (self.body_111)

    def body_111 (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (2, 3, 4, 5)
        src1 = gr.vector_source_i (src_data)
        op = gr.add_const_ii (2)
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op)
        fg.connect (op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        
    def test_111v_scheduler (self):
        self.leak_check (self.body_111v)

    def body_111v (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (2, 3, 4, 5)
        src1 = gr.vector_source_i (src_data)
        op = gr.add_const_ii (2)
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        
    def test_112 (self):
        fg = self.fg
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        nop3 = gr.nop (gr.sizeof_int)
        fg.connect ((nop1, 0), (nop2, 0))
        fg.connect ((nop1, 1), (nop3, 0))
        fg._setup_connections ()
        self.assertEqual (2, nop1.detail().noutputs())

    def test_113 (self):
        fg = self.fg
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        nop3 = gr.nop (gr.sizeof_int)
        fg.connect ((nop1, 0), (nop2, 0))
        fg.connect ((nop1, 0), (nop3, 0))
        fg._setup_connections ()
        self.assertEqual (1, nop1.detail().noutputs())

    def test_200_partition (self):
        self.leak_check (self.body_200)

    def body_200 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        fg.connect (nop1, dst1)
        fg.connect (src1, nop1)
        fg.validate ()
        p = fg.partition_graph (fg.all_blocks ())
        self.assertEqual ([[src1, nop1, dst1]], p)
        self.assertEqual ((3,0,0,0), all_counts ())
        
    def test_201_partition (self):
        self.leak_check (self.body_201)

    def body_201 (self):
        fg = self.fg
        src1 = gr.null_source (gr.sizeof_int)
        src2 = gr.null_source (gr.sizeof_int)
        nop1 = gr.nop (gr.sizeof_int)
        nop2 = gr.nop (gr.sizeof_int)
        dst1 = gr.null_sink (gr.sizeof_int)
        dst2 = gr.null_sink (gr.sizeof_int)
        
        fg.connect (nop2, dst2)
        fg.connect (src1, nop1)
        fg.connect (src2, nop2)
        fg.connect (nop1, dst1)
        fg.validate ()
        p = fg.partition_graph (fg.all_blocks ())
        self.assertEqual ([[src2, nop2, dst2], [src1, nop1, dst1]], p)
        self.assertEqual ((6,0,0,0), all_counts ())
        
    def test_300_hier (self):
        self.leak_check (self.body_300_hier)

    def body_300_hier (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (10, 11, 12, 13)
        src1 = gr.vector_source_i (src_data)
        op = wrap_add (fg, 10)
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        
    def test_301_hier (self):
        self.leak_check (self.body_301_hier)

    def body_301_hier (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (5, 8, 11, 14)
        src1 = gr.vector_source_i (src_data)
        op = mult_add (fg, 3, 5)
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        
    def test_302_hier (self):
        self.leak_check (self.body_302_hier)

    def body_302_hier (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (10, 11, 12, 13)
        src1 = gr.vector_source_i (src_data)
        op = gr.compose (fg, gr.add_const_ii (10))
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)

    def test_303_hier (self):
        self.leak_check (self.body_303_hier)

    def body_303_hier (self):
        fg = self.fg
        src_data = (0, 1, 2, 3)
        expected_result = (35, 38, 41, 44)
        src1 = gr.vector_source_i (src_data)
        op = gr.compose (fg, gr.add_const_ii (10), mult_add (fg, 3, 5))
        dst1 = gr.vector_sink_i ()
        fg.connect (src1, op, dst1)
        fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
        

if __name__ == '__main__':
    gr_unittest.main ()
