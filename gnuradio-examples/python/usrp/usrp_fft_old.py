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
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, fftsink, scopesink
from optparse import OptionParser
import wx

class app_flow_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser (option_class=eng_option)
        parser.add_option ("-d", "--decim", type="int", default=16,
                           help="set fgpa decimation rate to DECIM")
        parser.add_option ("-c", "--ddc-freq", type="eng_float", default=0,
                           help="set Rx DDC frequency to FREQ", metavar="FREQ")
        parser.add_option ("-m", "--mux", type="intx", default=0x32103210,
                           help="set fpga FR_RX_MUX register to MUX")
        parser.add_option ("-g", "--gain", type="eng_float", default=0,
                           help="set Rx PGA gain in dB (default 0 dB)")
        (options, args) = parser.parse_args ()

        self.u = usrp.source_c (0, options.decim, 1, gru.hexint(options.mux), 0)
        self.u.set_rx_freq (0, options.ddc_freq)

        self.u.set_pga (0, options.gain)
        self.u.set_pga (1, options.gain)

        self.u.set_verbose (0)
        
        input_rate = self.u.adc_freq () / self.u.decim_rate ()

        fft = fftsink.fft_sink_c (self, panel, fft_size=1024, sample_rate=input_rate)
        #fft = fftsink.fft_sink_c (self, panel, fft_size=1024, fft_rate=50, sample_rate=input_rate)
        self.connect (self.u, fft)
        vbox.Add (fft.win, 10, wx.EXPAND)

	if 0:
            c2f_1 = gr.complex_to_float ()
            scope = scopesink.scope_sink_f (self, panel, "Rx Data", input_rate)
            vbox.Add (scope.win, 4, wx.EXPAND)

            self.connect (self.u,c2f_1)
            self.connect ((c2f_1, 0), (scope, 0))
            self.connect ((c2f_1, 1), (scope, 1))

        # build small control area at bottom
        hbox = wx.BoxSizer (wx.HORIZONTAL)
        hbox.Add ((1, 1), 1, wx.EXPAND)
        hbox.Add (wx.StaticText (panel, -1, "Set ddc freq: "), 0, wx.ALIGN_CENTER)
        self.tc_freq = wx.TextCtrl (panel, -1, "", style=wx.TE_PROCESS_ENTER)
        hbox.Add (self.tc_freq, 0, wx.ALIGN_CENTER)
        wx.EVT_TEXT_ENTER (self.tc_freq, self.tc_freq.GetId(), self.handle_text_enter)
        hbox.Add ((1, 1), 1, wx.EXPAND)
        # add it to the main vbox
        vbox.Add (hbox, 0, wx.EXPAND)

        self.update_status_bar ()

    def handle_text_enter (self, event):
        str = event.GetString ()
        self.tc_freq.Clear ()
        self.u.set_rx_freq (0, eng_notation.str_to_num (str))
        self.update_status_bar ()

    def update_status_bar (self):
        ddc_freq = self.u.rx_freq (0)
        decim_rate = self.u.decim_rate ()
        sample_rate = self.u.adc_freq () / decim_rate
        msg = "decim: %d  %sS/s  DDC: %s" % (
            decim_rate,
            eng_notation.num_to_str (sample_rate),
            eng_notation.num_to_str (ddc_freq))
            
        self.frame.GetStatusBar().SetStatusText (msg, 1)

        

def main ():
    app = stdgui.stdapp (app_flow_graph, "USRP FFT")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
