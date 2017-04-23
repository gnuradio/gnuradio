#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2017 Free Software Foundation, Inc.
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
from gnuradio import blocks

class qa_concatenate (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_t (self):
        # set up fg
        dut = blocks.concatenate(gr.sizeof_gr_complex, 3)
        src0 = blocks.vector_source_c([1,2,4,5])
        src1 = blocks.vector_source_c([9,9,9,9])
        src2 = blocks.vector_source_c([0,0,0,0,0,0,0,0,0,0,1])
        dst = blocks.vector_sink_c()
        self.tb.connect(src0, (dut,0))
        self.tb.connect(src1, (dut,1))
        self.tb.connect(src2, (dut,2))
        self.tb.connect(dut, dst)
        self.tb.run ()
        # check data
        self.assertEqual(dst.data() , ((1+0j), (2+0j), (4+0j), (5+0j), (9+0j), (9+0j), (9+0j), (9+0j), 0j, 0j, 0j, 0j, 0j, 0j, 0j, 0j, 0j, 0j, (1+0j)))


if __name__ == '__main__':
    gr_unittest.run(qa_concatenate, "qa_concatenate.xml")
