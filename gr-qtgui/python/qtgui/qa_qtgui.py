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


class test_qtgui(gr_unittest.TestCase):

    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    # Tests to make sure we can instantiate the sink.
    # We use '5' in place of fft.window.WIN_BLACKMAN_hARRIS so we
    # don't have to worry about importing fft just for this one
    # constant.
    def test01(self):
        self.qtsnk = qtgui.sink_c(1024, 5,
                                  0.0, 1.0, "Test",
                                  True, True, True, True,
                                  None)

    def test02(self):
        self.qtsnk = qtgui.sink_f(1024, 5,
                                  0, 1, "Test",
                                  True, True, True, True,
                                  None)

    def test03(self):
        self.qtsnk = qtgui.time_sink_c(1024, 1, "Test", 1, None)

    def test04(self):
        self.qtsnk = qtgui.time_sink_f(1024, 1, "Test", 1, None)

    def test05(self):
        self.qtsnk = qtgui.freq_sink_c(1024, 5,
                                       0, 1, "Test", 1, None)

    def test06(self):
        self.qtsnk = qtgui.freq_sink_f(1024, 5,
                                       0, 1, "Test", 1, None)

    def test07(self):
        self.qtsnk = qtgui.waterfall_sink_c(1024, 5,
                                            0, 1, "Test", 1, None)

    def test08(self):
        self.qtsnk = qtgui.waterfall_sink_f(1024, 5,
                                            0, 1, "Test", 1, None)

    def test09(self):
        self.qtsnk = qtgui.const_sink_c(1024, "Test", 1, None)

    def test10(self):
        self.qtsnk = qtgui.time_raster_sink_b(1024, 100, 100.5,
                                              [], [], "Test", 1, None)

    def test11(self):
        self.qtsnk = qtgui.time_raster_sink_f(1024, 100, 100.5,
                                              [], [], "Test", 1, None)

    def test12(self):
        self.qtsnk = qtgui.histogram_sink_f(1024, 100, -1, 1, "Test", 1, None)

    def test13(self):
        self.qtsnk = qtgui.eye_sink_f(1024, 1, 1, None)

    def test14(self):
        self.qtsnk = qtgui.eye_sink_c(1024, 1, 1, None)


if __name__ == '__main__':
    gr_unittest.run(test_qtgui)
