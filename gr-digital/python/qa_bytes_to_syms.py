#!/usr/bin/env python
#
# Copyright 2004,2007,2010,2012 Free Software Foundation, Inc.
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
import digital_swig as digital
import math

class test_bytes_to_syms (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_bytes_to_syms_001 (self):
        src_data = (0x01, 0x80, 0x03)
        expected_result = (-1, -1, -1, -1, -1, -1, -1, +1,
                           +1, -1, -1, -1, -1, -1, -1, -1,
                           -1, -1, -1, -1, -1, -1, +1, +1)
        src = gr.vector_source_b (src_data)
        op = digital.bytes_to_syms ()
        dst = gr.vector_sink_f ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertEqual (expected_result, result_data)

if __name__ == '__main__':
    gr_unittest.run(test_bytes_to_syms, "test_bytes_to_syms.xml")

