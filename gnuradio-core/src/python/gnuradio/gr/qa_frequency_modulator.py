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

def sincos(x):
    return  math.cos(x) + math.sin(x) * 1j


class test_frequency_modulator (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    def test_fm_001 (self):
        pi = math.pi
        sensitivity = pi/4
        src_data = (1.0/4, 1.0/2, 1.0/4, -1.0/4, -1.0/2, -1/4.0)
        running_sum = (pi/16, 3*pi/16, pi/4, 3*pi/16, pi/16, 0)
        expected_result = tuple ([sincos (x) for x in running_sum])
        src = gr.vector_source_f (src_data)
        op = gr.frequency_modulator_fc (sensitivity)
        dst = gr.vector_sink_c ()
        self.tb.connect (src, op)
        self.tb.connect (op, dst)
        self.tb.run ()
        result_data = dst.data ()
        self.assertComplexTuplesAlmostEqual (expected_result, result_data, 5)


if __name__ == '__main__':
    gr_unittest.run(test_frequency_modulator, "test_frequency_modulator.xml")
        
