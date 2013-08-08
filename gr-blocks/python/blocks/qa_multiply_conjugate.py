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

from gnuradio import gr, gr_unittest, blocks

class test_multiply_conjugate (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_000 (self):
        src_data0 = (-2-2j, -1-1j, -2+2j, -1+1j,
                      2-2j,  1-1j,  2+2j,  1+1j,
                      0+0j)
        src_data1 = (-3-3j, -4-4j, -3+3j, -4+4j,
                      3-3j,  4-4j,  3+3j,  4+4j,
                      0+0j)

        exp_data = (12+0j, 8+0j, 12+0j, 8+0j,
                    12+0j, 8+0j, 12+0j, 8+0j,
                    0+0j)
        src0 = blocks.vector_source_c(src_data0)
        src1 = blocks.vector_source_c(src_data1)
        op = blocks.multiply_conjugate_cc ()
        dst = blocks.vector_sink_c ()

        self.tb.connect(src0, (op,0))
        self.tb.connect(src1, (op,1))
        self.tb.connect(op, dst)
        self.tb.run()
        result_data = dst.data ()
        self.assertEqual (exp_data, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_multiply_conjugate, "test_multiply_conjugate.xml")
