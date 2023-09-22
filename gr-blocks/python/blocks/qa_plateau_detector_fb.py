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


class qa_plateau_detector_fb (gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_t(self):
        #                  | Spur spike 1     | Plateau                  | Spur spike 2
        test_signal = [0, 1, .2, .4, .6, .8, 1, 1, 1, 1, 1, .8, .6, .4, 1, 0]
        expected_sig = [0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0]
        #                                           | Center of Plateau
        sink = blocks.vector_sink_b()
        self.tb.connect(blocks.vector_source_f(test_signal),
                        blocks.plateau_detector_fb(5), sink)
        self.tb.run()
        self.assertEqual(expected_sig, sink.data())


if __name__ == '__main__':
    gr_unittest.run(qa_plateau_detector_fb)
