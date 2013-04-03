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

from gnuradio import gr, gr_unittest, digital, blocks

class test_probe_density(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        src_data = [0, 1, 0, 1]
        expected_data = 1
        src = blocks.vector_source_b(src_data)
        op = digital.probe_density_b(1)
        self.tb.connect(src, op)
        self.tb.run()

        result_data = op.density()
        self.assertEqual(expected_data, result_data)


    def test_002(self):
        src_data = [1, 1, 1, 1]
        expected_data = 1
        src = blocks.vector_source_b(src_data)
        op = digital.probe_density_b(0.01)
        self.tb.connect(src, op)
        self.tb.run()

        result_data = op.density()
        self.assertEqual(expected_data, result_data)

    def test_003(self):
        src_data = [0, 1, 0, 1, 0, 1, 0, 1, 0, 1]
        expected_data = 0.95243
        src = blocks.vector_source_b(src_data)
        op = digital.probe_density_b(0.01)
        self.tb.connect(src, op)
        self.tb.run()

        result_data = op.density()
        print result_data
        self.assertAlmostEqual(expected_data, result_data, 5)

if __name__ == '__main__':
    gr_unittest.run(test_probe_density, "test_probe_density.xml")
        
