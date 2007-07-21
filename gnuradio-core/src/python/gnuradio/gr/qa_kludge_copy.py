#!/usr/bin/env python
#
# Copyright 2006 Free Software Foundation, Inc.
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
import math
import random


class test_kludge_copy(gr_unittest.TestCase):

    def setUp(self):
        self.fg = gr.flow_graph()
        self.rng = random.Random()
        self.rng.seed(0)

    def tearDown(self):
        self.fg = None
        self.rng = None

    def make_random_int_tuple(self, L):
        result = []
        for x in range(L):
            result.append(self.rng.randint(int(-1e9), int(+1e9)))
        return tuple(result)


    def test_001(self):
        # 1 input stream; 1 output stream
        src0_data = self.make_random_int_tuple(16000)
        src0 = gr.vector_source_i(src0_data)
        op = gr.kludge_copy(gr.sizeof_int)
        dst0 = gr.vector_sink_i()
        self.fg.connect(src0, op, dst0)
        self.fg.run()
        dst0_data = dst0.data()
        self.assertEqual(src0_data, dst0_data)

    def test_002(self):
        # 2 input streams; 2 output streams
        src0_data = self.make_random_int_tuple(16000)
        src1_data = self.make_random_int_tuple(16000)
        src0 = gr.vector_source_i(src0_data)
        src1 = gr.vector_source_i(src1_data)
        op = gr.kludge_copy(gr.sizeof_int)
        dst0 = gr.vector_sink_i()
        dst1 = gr.vector_sink_i()
        self.fg.connect(src0, (op, 0), dst0)
        self.fg.connect(src1, (op, 1), dst1)
        self.fg.run()
        dst0_data = dst0.data()
        dst1_data = dst1.data()
        self.assertEqual(src0_data, dst0_data)
        self.assertEqual(src1_data, dst1_data)

    def test_003(self):
        # number of input streams != number of output streams
        src0_data = self.make_random_int_tuple(16000)
        src1_data = self.make_random_int_tuple(16000)
        src0 = gr.vector_source_i(src0_data)
        src1 = gr.vector_source_i(src1_data)
        op = gr.kludge_copy(gr.sizeof_int)
        dst0 = gr.vector_sink_i()
        dst1 = gr.vector_sink_i()
        self.fg.connect(src0, (op, 0), dst0)
        self.fg.connect(src1, (op, 1))
        self.assertRaises(ValueError, self.fg.run)

if __name__ == '__main__':
    gr_unittest.main ()
    
