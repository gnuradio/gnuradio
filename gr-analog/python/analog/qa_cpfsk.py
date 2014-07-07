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

import math

from gnuradio import gr, gr_unittest, analog, blocks

class test_cpfsk_bc(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_cpfsk_bc_001(self):
        # Test set/gets

        op = analog.cpfsk_bc(2, 1, 2)

        op.set_amplitude(2)
        a = op.amplitude()
        self.assertEqual(2, a)

        freq = 2*math.pi/2.0
        f = op.freq()
        self.assertAlmostEqual(freq, f, 5)

        p = op.phase()
        self.assertEqual(0, p)

    def test_cpfsk_bc_002(self):
        src_data = 10*[0, 1]
        expected_result = map(lambda x: complex(2*x-1,0), src_data)

        src = blocks.vector_source_b(src_data)
        op = analog.cpfsk_bc(2, 1, 2)
        dst = blocks.vector_sink_c()

        self.tb.connect(src, op)
        self.tb.connect(op, dst)
        self.tb.run()

        result_data = dst.data()[0:len(expected_result)]
        self.assertComplexTuplesAlmostEqual(expected_result, result_data, 4)

if __name__ == '__main__':
    gr_unittest.run(test_cpfsk_bc, "test_cpfsk_bc.xml")

