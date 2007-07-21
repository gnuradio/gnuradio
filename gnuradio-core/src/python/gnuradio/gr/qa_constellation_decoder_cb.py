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
import math

class test_head (gr_unittest.TestCase):

    def setUp (self):
        self.fg = gr.flow_graph ()

    def tearDown (self):
        self.fg = None

    def test_constellation_decoder_cb (self):
        symbol_positions  = [1 + 0j, 0 + 1j , -1 + 0j, 0 - 1j]
        symbol_values_out = [0, 1, 2, 3]
	expected_result = (       0,          3,           2,            1,        0,       0,          3)
  	src_data =        (0.5 + 0j, 0.1 - 1.2j, -0.8 - 0.1j, -0.45 + 0.8j, 0.8 - 0j, 0.5 + 0j, 0.1 - 1.2j)
        src = gr.vector_source_c (src_data)
        op = gr.constellation_decoder_cb (symbol_positions, symbol_values_out)
        dst = gr.vector_sink_b ()
        self.fg.connect (src, op)
        self.fg.connect (op, dst)
        self.fg.run ()               # run the graph and wait for it to finish
        actual_result = dst.data ()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual (expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.main ()

