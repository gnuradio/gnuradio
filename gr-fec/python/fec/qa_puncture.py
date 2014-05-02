#!/usr/bin/env python
#
# Copyright 2014 Free Software Foundation, Inc.
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
from collections import deque

class test_puncture (gr_unittest.TestCase):

    def puncture_setup(self):
        p = []
        for i in range(self.puncsize):
            p.append(self.puncpat >> (self.puncsize - 1 - i) & 1)
        d = deque(p)
        d.rotate(self.delay)
        _puncpat = list(d)

        self.expected = []
        for n in range(len(self.src_data)/self.puncsize):
            for i in range(self.puncsize):
                if _puncpat[i] == 1:
                    self.expected.append(self.src_data[n*self.puncsize+i]);

    def setUp(self):
        self.src_data = 10000*range(64)
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_000(self):
        # Test normal operation of the puncture block

        self.puncsize = 8
        self.puncpat = 0xEF
        self.delay = 0

        self.puncture_setup()

        src = blocks.vector_source_b(self.src_data)
	op  = fec.puncture_bb(self.puncsize, self.puncpat, self.delay)
	dst = blocks.vector_sink_b()

	self.tb.connect(src, op, dst)
	self.tb.run()

	dst_data = list(dst.data())
        for i in xrange(len(dst_data)):
            dst_data[i] = int(dst_data[i])

        self.assertEqual(self.expected, dst_data)


    def test_001(self):
        # Test normal operation of the puncture block with a delay

        self.puncsize = 8
        self.puncpat = 0xEE
        self.delay = 1

        self.src_data = range(16)

        self.puncture_setup()

        src = blocks.vector_source_b(self.src_data)
	op  = fec.puncture_bb(self.puncsize, self.puncpat, self.delay)
	dst = blocks.vector_sink_b()

	self.tb.connect(src, op, dst)
	self.tb.run()

	dst_data = list(dst.data())
        for i in xrange(len(dst_data)):
            dst_data[i] = int(dst_data[i])

        self.assertEqual(self.expected, dst_data)


    def test_002(self):
        # Test scenario where we have defined a puncture pattern with
        # more bits than the puncsize.

        self.puncsize = 4
        self.puncpat = 0x5555
        self.delay = 0

        self.puncture_setup()

        src = blocks.vector_source_b(self.src_data)
	op  = fec.puncture_bb(self.puncsize, self.puncpat, self.delay)
	dst = blocks.vector_sink_b()

	self.tb.connect(src, op, dst)
	self.tb.run()

	dst_data = list(dst.data())
        for i in xrange(len(dst_data)):
            dst_data[i] = int(dst_data[i])

        self.assertEqual(self.expected, dst_data)

    def test_003(self):
        # Test scenario where we have defined a puncture pattern with
        # more bits than the puncsize with a delay. The python code
        # doesn't account for this when creating self.expected, but
        # this should be equivalent to a puncpat of the correct size.

        self.puncsize = 4
        self.puncpat0 = 0x5555 # too many bits set
        self.puncpat1 = 0x55   # num bits = puncsize
        self.delay = 1

        src = blocks.vector_source_b(self.src_data)
	op0  = fec.puncture_bb(self.puncsize, self.puncpat0, self.delay)
	op1  = fec.puncture_bb(self.puncsize, self.puncpat1, self.delay)
	dst0 = blocks.vector_sink_b()
	dst1 = blocks.vector_sink_b()

	self.tb.connect(src, op0, dst0)
	self.tb.connect(src, op1, dst1)
	self.tb.run()

	dst_data0 = list(dst0.data())
        for i in xrange(len(dst_data0)):
            dst_data0[i] = int(dst_data0[i])

	dst_data1 = list(dst1.data())
        for i in xrange(len(dst_data1)):
            dst_data1[i] = int(dst_data1[i])

        self.assertEqual(dst_data1, dst_data0)



    def test_f_000(self):
        # Test normal operation of the float puncture block

        self.puncsize = 8
        self.puncpat = 0xEF
        self.delay = 0

        self.puncture_setup()

        src = blocks.vector_source_f(self.src_data)
	op  = fec.puncture_ff(self.puncsize, self.puncpat, self.delay)
	dst = blocks.vector_sink_f()

	self.tb.connect(src, op, dst)
	self.tb.run()

	dst_data = list(dst.data())
        self.assertEqual(self.expected, dst_data)


    def test_f_001(self):
        # Test normal operation of the puncture block with a delay

        self.puncsize = 8
        self.puncpat = 0xEE
        self.delay = 1

        self.src_data = range(16)

        self.puncture_setup()

        src = blocks.vector_source_f(self.src_data)
	op  = fec.puncture_ff(self.puncsize, self.puncpat, self.delay)
	dst = blocks.vector_sink_f()

	self.tb.connect(src, op, dst)
	self.tb.run()

	dst_data = list(dst.data())
        self.assertEqual(self.expected, dst_data)


    def test_f_002(self):
        # Test scenariou where we have defined a puncture pattern with
        # more bits than the puncsize.

        self.puncsize = 4
        self.puncpat = 0x5555
        self.delay = 0

        self.puncture_setup()

        src = blocks.vector_source_f(self.src_data)
	op  = fec.puncture_ff(self.puncsize, self.puncpat, self.delay)
	dst = blocks.vector_sink_f()

	self.tb.connect(src, op, dst)
	self.tb.run()

	dst_data = list(dst.data())
        self.assertEqual(self.expected, dst_data)

    def test_f_003(self):
        # Test scenariou where we have defined a puncture pattern with
        # more bits than the puncsize with a delay. The python code
        # doesn't account for this when creating self.expected, but
        # this should be equivalent to a puncpat of the correct size.

        self.puncsize = 4
        self.puncpat0 = 0x5555 # too many bits set
        self.puncpat1 = 0x55   # num bits = puncsize
        self.delay = 1

        src = blocks.vector_source_f(self.src_data)
	op0  = fec.puncture_ff(self.puncsize, self.puncpat0, self.delay)
	op1  = fec.puncture_ff(self.puncsize, self.puncpat1, self.delay)
	dst0 = blocks.vector_sink_f()
	dst1 = blocks.vector_sink_f()

	self.tb.connect(src, op0, dst0)
	self.tb.connect(src, op1, dst1)
	self.tb.run()

	dst_data0 = list(dst0.data())
	dst_data1 = list(dst1.data())

        self.assertEqual(dst_data1, dst_data0)

if __name__ == '__main__':
    gr_unittest.run(test_puncture, "test_puncture.xml")
