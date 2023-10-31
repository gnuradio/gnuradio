#!/usr/bin/env python
#
# Copyright 2011, 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, qtgui


class test_qtgui_video_display(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test01(self):
        self.qtsnk = qtgui.video_display(gr.sizeof_int, None)


if __name__ == '__main__':
    gr_unittest.run(test_qtgui_video_display)
