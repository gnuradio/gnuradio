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

from gnuradio import gr, gr_unittest
import qtgui_swig

class test_qtgui(gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

    def tearDown (self):
        self.tb = None
    
    def test01 (self):
        # Test to make sure we can instantiate the sink
        self.qtsnk = qtgui_swig.sink_c(1024, gr.firdes.WIN_BLACKMAN_hARRIS, 
                                       0, 1, "Test",
                                       True, True, True, True)

    def test02 (self):
        # Test to make sure we can instantiate the sink
        self.qtsnk = qtgui_swig.sink_f(1024, gr.firdes.WIN_BLACKMAN_hARRIS, 
                                       0, 1, "Test",
                                       True, True, True, True)
        
        
if __name__ == '__main__':
    gr_unittest.run(test_qtgui, "test_qtgui.xml")
