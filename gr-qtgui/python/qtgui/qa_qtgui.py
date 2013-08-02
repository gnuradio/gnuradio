#!/usr/bin/env python
#
# Copyright 2011 Free Software Foundation, Inc.
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

from gnuradio import gr, gr_unittest, qtgui

class test_qtgui(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None

    # Tests to make sure we can instantiate the sink.
    # We use '5' in place of filter.firdes.WIN_BLACKMAN_hARRIS so we
    # don't have to worry about importing filter just for this one
    # constant.
    def test01(self):
        self.qtsnk = qtgui.sink_c(1024, 5,
                                  0, 1, "Test",
                                  True, True, True, True)

    def test02(self):
        self.qtsnk = qtgui.sink_f(1024, 5,
                                  0, 1, "Test",
                                  True, True, True, True)

    def test03(self):
        self.qtsnk = qtgui.time_sink_c(1024, 1, "Test", 1)

    def test04(self):
        self.qtsnk = qtgui.time_sink_f(1024, 1, "Test", 1)

    def test05(self):
        self.qtsnk = qtgui.freq_sink_c(1024, 5,
                                       0, 1, "Test", 1)

    def test06(self):
        self.qtsnk = qtgui.freq_sink_f(1024, 5,
                                       0, 1, "Test", 1)

    def test07(self):
        self.qtsnk = qtgui.waterfall_sink_c(1024, 5,
                                            0, 1, "Test")

    def test08(self):
        self.qtsnk = qtgui.waterfall_sink_f(1024, 5,
                                            0, 1, "Test")

    def test09(self):
        self.qtsnk = qtgui.const_sink_c(1024, "Test", 1)

    def test10(self):
        self.qtsnk = qtgui.time_raster_sink_b(1024, 100, 100.5,
                                              [], [], "Test", 1)

    def test11(self):
        self.qtsnk = qtgui.time_raster_sink_f(1024, 100, 100.5,
                                              [], [], "Test", 1)

    def test12(self):
        self.qtsnk = qtgui.histogram_sink_f(1024, 100, -1, 1, "Test", 1)

if __name__ == '__main__':
    gr_unittest.run(test_qtgui, "test_qtgui.xml")
