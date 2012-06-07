#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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

class test_vector_insert(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001(self):
        src_data = [float(x) for x in range(16)]
        expected_result = tuple(src_data)

        period = 9177;
        offset = 0;

        src = gr.null_source(1)
        head = gr.head(1, 10000000);
        ins = gr.vector_insert_b([1], period, offset);
        dst = gr.vector_sink_b()

        self.tb.connect(src, head, ins, dst)
        self.tb.run()
        result_data = dst.data()

        for i in range(10000):
            if(i%period == offset):
                self.assertEqual(1, result_data[i])
            else:
                self.assertEqual(0, result_data[i])

if __name__ == '__main__':
    gr_unittest.run(test_vector_insert, "test_vector_insert.xml")

