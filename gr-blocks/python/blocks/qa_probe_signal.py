#!/usr/bin/env python
#
# Copyright 2012-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
