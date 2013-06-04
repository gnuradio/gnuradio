#!/usr/bin/env python
#
# Copyright 2012-2013 Free Software Foundation, Inc.
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

class test_probe_signal(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001(self):
        value = 12.3
        repeats = 100
        src_data = [value] * repeats

        src = blocks.vector_source_f(src_data)
        dst = blocks.probe_signal_f()

        self.tb.connect(src, dst)
        self.tb.run()
        output = dst.level()
        self.assertAlmostEqual(value, output, places=6)

    def test_002(self):
        vector_length = 10
        repeats = 10
        value = [0.5+i for i in range(0, vector_length)]
        src_data = value * repeats

        src = blocks.vector_source_f(src_data)
        s2v = blocks.stream_to_vector(gr.sizeof_float, vector_length)
        dst = blocks.probe_signal_vf(vector_length)

        self.tb.connect(src, s2v, dst)
        self.tb.run()
        output = dst.level()
        self.assertEqual(len(output), vector_length)
        self.assertAlmostEqual(value[3], output[3], places=6)

if __name__ == '__main__':
    gr_unittest.run(test_probe_signal, "test_probe_signal.xml")
