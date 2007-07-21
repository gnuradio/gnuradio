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

class test_head (gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.flow_graph ()

    def tearDown (self):
        self.fg = None

    def test_head (self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        expected_result = (1, 2, 3, 4)
        src1 = gr.vector_source_i (src_data)
        op = gr.head (gr.sizeof_int, 4)
        dst1 = gr.vector_sink_i ()
        self.fg.connect (src1, op)
        self.fg.connect (op, dst1)
        self.fg.run ()
        dst_data = dst1.data ()
        self.assertEqual (expected_result, dst_data)
    

if __name__ == '__main__':
    gr_unittest.main ()
