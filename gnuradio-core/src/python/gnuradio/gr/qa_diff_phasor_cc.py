#!/usr/bin/env python
#
# Copyright 2004,2007,2010 Free Software Foundation, Inc.
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

class test_diff_phasor (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_diff_phasor_cc (self):
        src_data = (0+0j, 1+0j, -1+0j, 3+4j, -3-4j, -3+4j)
        expected_result = (0+0j, 0+0j, -1+0j, -3-4j, -25+0j, -7-24j)
        src = gr.vector_source_c (src_data)
        op = gr.diff_phasor_cc ()
        dst = gr.vector_sink_c ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()               # run the graph and wait for it to finish
        actual_result = dst.data ()  # fetch the contents of the sink
        self.assertComplexTuplesAlmostEqual (expected_result, actual_result)



if __name__ == '__main__':
    gr_unittest.run(test_diff_phasor, "test_diff_phasor.xml")

