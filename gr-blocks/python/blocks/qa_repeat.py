#!/usr/bin/env python
#
# Copyright 2008,2010,2012,2013 Free Software Foundation, Inc.
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

class test_repeat (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_001_float(self):
	src_data = [n*1.0 for n in range(100)];
	dst_data = []
	for n in range(100):
	    dst_data += [1.0*n, 1.0*n, 1.0*n]

	src = blocks.vector_source_f(src_data)
	rpt = blocks.repeat(gr.sizeof_float, 3)
	dst = blocks.vector_sink_f()
	self.tb.connect(src, rpt, dst)
	self.tb.run()
	self.assertFloatTuplesAlmostEqual(dst_data, dst.data(), 6)

if __name__ == '__main__':
    gr_unittest.run(test_repeat, "test_repeat.xml")
