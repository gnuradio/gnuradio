#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2013 Free Software Foundation, Inc.
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
import fec_swig as fec
import blocks_swig as blocks

class test_ccsds_27 (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def xtest_ccsds_27 (self):
        src_data = (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
	expected = (0, 0, 0, 0, 1, 2, 3, 4, 5, 6)
        src = blocks.vector_source_b(src_data)
	enc = fec.encode_ccsds_27_bb()
	b2f = blocks.char_to_float()
	add = blocks.add_const_ff(-0.5)
	mul = blocks.multiply_const_ff(2.0)
	dec = fec.decode_ccsds_27_fb()
	dst = blocks.vector_sink_b()
	self.tb.connect(src, enc, b2f, add, mul, dec, dst)
	self.tb.run()
	dst_data = dst.data()
        self.assertEqual(expected, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_ccsds_27, "test_ccsds_27.xml")
