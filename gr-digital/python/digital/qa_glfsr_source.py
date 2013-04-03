#!/usr/bin/env python
#
# Copyright 2007,2010,2012,2013 Free Software Foundation, Inc.
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

class test_glfsr_source(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000_make_b(self):
        src = digital.glfsr_source_b(16)
        self.assertEquals(src.mask(), 0x8016)
        self.assertEquals(src.period(), 2**16-1)

    def test_001_degree_b(self):
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_b(0))
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_b(33))

    def test_002_correlation_b(self):
        for degree in range(1,11):                # Higher degrees take too long to correlate
            src = digital.glfsr_source_b(degree, False)
            b2f = digital.chunks_to_symbols_bf((-1.0,1.0), 1)
            dst = blocks.vector_sink_f()
	    del self.tb # Discard existing top block
	    self.tb = gr.top_block()
            self.tb.connect(src, b2f, dst)
            self.tb.run()
	    self.tb.disconnect_all()
            actual_result = dst.data()
            R = auto_correlate(actual_result)
            self.assertEqual(R[0], float(len(R))) # Auto-correlation peak at origin
            for i in range(len(R)-1):
                self.assertEqual(R[i+1], -1.0)    # Auto-correlation minimum everywhere else

    def test_003_make_f(self):
        src = digital.glfsr_source_f(16)
        self.assertEquals(src.mask(), 0x8016)
        self.assertEquals(src.period(), 2**16-1)

    def test_004_degree_f(self):
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_f(0))
        self.assertRaises(RuntimeError,
                          lambda: digital.glfsr_source_f(33))
    def test_005_correlation_f(self):
        for degree in range(1,11):                # Higher degrees take too long to correlate
            src = digital.glfsr_source_f(degree, False)
            dst = blocks.vector_sink_f()
	    del self.tb # Discard existing top block
	    self.tb = gr.top_block()
            self.tb.connect(src, dst)
            self.tb.run()

            actual_result = dst.data()
            R = auto_correlate(actual_result)
            self.assertEqual(R[0], float(len(R))) # Auto-correlation peak at origin
            for i in range(len(R)-1):
                self.assertEqual(R[i+1], -1.0)    # Auto-correlation minimum everywhere else

def auto_correlate(data):
    l = len(data)
    R = [0,]*l
    for lag in range(l):
        for i in range(l):
            R[lag] += data[i]*data[i-lag]
    return R

if __name__ == '__main__':
    gr_unittest.run(test_glfsr_source, "test_glfsr_source.xml")
