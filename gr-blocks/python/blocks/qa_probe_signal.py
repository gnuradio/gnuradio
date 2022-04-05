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
        value = [0.5 + i for i in range(0, vector_length)]
        src_data = value * repeats

        src = blocks.vector_source_f(src_data)
        s2v = blocks.stream_to_vector(gr.sizeof_float, vector_length)
        dst = blocks.probe_signal_vf(vector_length)

        self.tb.connect(src, s2v, dst)
        self.tb.run()
        output = dst.level()
        self.assertEqual(len(output), vector_length)
        self.assertAlmostEqual(value[3], output[3], places=6)

    def test_003_race_condition_regression_test(self):
        src = blocks.vector_source_c([1 + 2j, 3 + 4j], True)
        dst = blocks.probe_signal_c()

        self.tb.connect(src, dst)
        self.tb.start()

        while dst.level() == 0.0:
            continue

        for _ in range(100000):
            output = dst.level()
            self.assertIn(output, [1 + 2j, 3 + 4j])

        self.tb.stop()
        self.tb.wait()

    def test_004_race_condition_regression_test_vector(self):
        vector_length = 10
        src_data = [1.0] * vector_length + [2.0] * vector_length

        src = blocks.vector_source_f(src_data, True, vector_length)
        dst = blocks.probe_signal_vf(vector_length)

        self.tb.connect(src, dst)
        self.tb.start()

        while dst.level()[0] == 0.0:
            continue

        for _ in range(10000):
            output = dst.level()
            self.assertIn(output[0], [1.0, 2.0])
            for i in range(1, vector_length):
                self.assertEqual(output[0], output[i])

        self.tb.stop()
        self.tb.wait()


if __name__ == '__main__':
    gr_unittest.run(test_probe_signal)
