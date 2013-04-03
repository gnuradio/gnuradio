#!/usr/bin/env python
#
# Copyright 2011-2013 Free Software Foundation, Inc.
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

import random

from gnuradio import gr, gr_unittest, digital, blocks

class test_binary_slicer_fb(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_binary_slicer_fb(self):
	expected_result = ( 0, 1,  0,  0, 1, 1,  0,  0,  0, 1, 1, 1,  0, 1, 1, 1, 1)
  	src_data =        (-1, 1, -1, -1, 1, 1, -1, -1, -1, 1, 1, 1, -1, 1, 1, 1, 1)
        src_data = [s + (1 - random.random()) for s in src_data] # add some noise
        src = blocks.vector_source_f(src_data)
        op = digital.binary_slicer_fb()
        dst = blocks.vector_sink_b()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()               # run the graph and wait for it to finish

        actual_result = dst.data()  # fetch the contents of the sink
	#print "actual result", actual_result
	#print "expected result", expected_result
        self.assertFloatTuplesAlmostEqual(expected_result, actual_result)


if __name__ == '__main__':
    gr_unittest.run(test_binary_slicer_fb, "test_binary_slicer_fb.xml")

