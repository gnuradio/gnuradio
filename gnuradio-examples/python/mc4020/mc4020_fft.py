#!/usr/bin/env python
#
# Copyright 2004 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr
from gnuradio import mc4020
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, fftsink
from optparse import OptionParser
import wx

class app_flow_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

        
        parser = OptionParser (option_class=eng_option)
        (options, args) = parser.parse_args ()

        input_rate = 20e6
        src = mc4020.source (input_rate, mc4020.MCC_CH3_EN | mc4020.MCC_ALL_1V)
        cvt = gr.short_to_float ()
        block, fft_win = fftsink.make_fft_sink_f (self, panel, "Rx Data", 512, input_rate)

        self.connect (src, cvt)
        self.connect (cvt, block)
        vbox.Add (fft_win, 1, wx.EXPAND)


def main ():
    app = stdgui.stdapp (app_flow_graph, "USRP FFT")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
