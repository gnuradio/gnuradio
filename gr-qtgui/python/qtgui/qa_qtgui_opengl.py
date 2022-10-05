#!/usr/bin/env python
#
# Copyright 2023 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from gnuradio import gr, gr_unittest, qtgui


class test_qtgui_opengl(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test01(self):
        self.qtsnk = qtgui.fosphor_display(1024, 64, 512)


if __name__ == '__main__':
    gr_unittest.run(test_qtgui_opengl)
