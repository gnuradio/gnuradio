#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, blocks


class test_peak_detector(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_01(self):
        tb = self.tb

        data = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0]

        expected_result = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        src = blocks.vector_source_f(data, False)
        regen = blocks.peak_detector_fb()
        dst = blocks.vector_sink_b()

        tb.connect(src, regen)
        tb.connect(regen, dst)
        tb.run()

        dst_data = dst.data()

        self.assertEqual(expected_result, dst_data)

    def test_02(self):
        tb = self.tb

        data = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0]

        expected_result = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        src = blocks.vector_source_i(data, False)
        regen = blocks.peak_detector_ib()
        dst = blocks.vector_sink_b()

        tb.connect(src, regen)
        tb.connect(regen, dst)
        tb.run()

        dst_data = dst.data()

        self.assertEqual(expected_result, dst_data)

    def test_03(self):
        tb = self.tb

        data = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                9, 8, 7, 6, 5, 4, 3, 2, 1, 0]

        expected_result = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

        src = blocks.vector_source_s(data, False)
        regen = blocks.peak_detector_sb()
        dst = blocks.vector_sink_b()

        tb.connect(src, regen)
        tb.connect(regen, dst)
        tb.run()

        dst_data = dst.data()

        self.assertEqual(expected_result, dst_data)


if __name__ == '__main__':
    gr_unittest.run(test_peak_detector)
