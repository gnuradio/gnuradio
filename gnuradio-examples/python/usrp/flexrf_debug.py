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

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, fftsink, scopesink, slider
from optparse import OptionParser
import wx

class app_flow_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser (option_class=eng_option)
        parser.add_option ("-d", "--decim", type="int", default=8,
                           help="set fgpa decimation rate to DECIM")
        parser.add_option ("-c", "--ddc-freq", type="eng_float", default=0,
                           help="set Digital downconverter frequency to FREQ", metavar="FREQ")
        parser.add_option ("-f", "--freq", type="eng_float", default=950e6,
                           help="set RF downconverter frequency to FREQ", metavar="FREQ")
        parser.add_option ("-m", "--mux", type="intx", default=0x32103210,
                           help="set fpga FR_RX_MUX register to MUX")
        parser.add_option ("-g", "--gain", type="eng_float", default=0,
                           help="set Rx PGA gain in dB (default 0 dB)")
        (options, args) = parser.parse_args ()

        self.u = usrp.source_c (0, options.decim, 1, gru.hexint(options.mux), 0)
        
        self.u.set_verbose (0)
        
        input_rate = self.u.adc_freq () / self.u.decim_rate ()

        block = fftsink.fft_sink_c (self, panel, fft_size=1024, sample_rate=input_rate)
        self.connect (self.u, block)
        vbox.Add (block.win, 10, wx.EXPAND)

	if 0:
            c2f_1 = gr.complex_to_float ()
            scope = scopesink.scope_sink_f (self, panel, "Rx Data", input_rate)
            vbox.Add (scope.win, 6, wx.EXPAND)

            self.connect (self.u,c2f_1)
            self.connect ((c2f_1, 0), (scope, 0))
            self.connect ((c2f_1, 1), (scope, 1))

        if 0:
            rms_complex = gr.rms_cf(.0001)
            rms_i = gr.rms_ff(.0001)
            rms_q = gr.rms_ff(.0001)
            
            self.connect(self.u,rms_complex)
            self.connect((c2f_1,0),rms_i)
            self.connect((c2f_1,1),rms_q)
            
            ns1 = gr.null_sink(4)
            ns2 = gr.null_sink(4)
            ns3 = gr.null_sink(4)
            
            self.connect(rms_complex,ns1)
            self.connect(rms_i,ns2)
            self.connect(rms_q,ns3)

        # sliders

        #vbox.Add(slider.slider(panel, 0, 104, self.set_gain), 1, wx.ALIGN_CENTER)

        #vbox.Add(slider.slider(panel, 0, 4095, self.set_gain_gc1), 1, wx.ALIGN_CENTER)
        #vbox.Add(slider.slider(panel, 0, 31, self.set_gain_gc2), 1, wx.ALIGN_CENTER)
        #vbox.Add(slider.slider(panel, 0, 1, self.set_gain_dl), 1, wx.ALIGN_CENTER)
        #vbox.Add(slider.slider(panel, 0, 200, self.set_gain_i), 1, wx.ALIGN_CENTER)
        #vbox.Add(slider.slider(panel, 0, 200, self.set_gain_q), 1, wx.ALIGN_CENTER)

        self.offset = 0
        #vbox.Add(slider.slider(panel, -200, 200, self.set_offset_i), 1, wx.ALIGN_CENTER)
        #vbox.Add(slider.slider(panel, -200, 200, self.set_offset_q), 1, wx.ALIGN_CENTER)

        vbox.Add(slider.slider(panel, 380, 480, self.set_rf_freq), 1, wx.EXPAND|wx.ALIGN_CENTER)
        vbox.Add(slider.slider(panel, -32000, +32000, self.set_if_freq), 1, wx.EXPAND|wx.ALIGN_CENTER)
        vbox.Add(slider.slider(panel, 0, 4095, self.set_gain), 1, wx.EXPAND|wx.ALIGN_CENTER)

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

    def set_rf_freq (self,freq):
        (success,actual_freq) = self.set_freq(1e6*freq)
        if not success:
            print "Failed on ",freq
    def set_if_freq (self,freq):
        self.u.set_rx_freq(0,freq*1e3)
            
    def set_gain (self,gain):
        self.rfrx.set_gain(gain)

    def set_gain_i (self,gain):
        self.u.set_pga(0,gain/10.0)
    def set_gain_q (self,gain):
        self.u.set_pga(1,gain/10.0)

    def set_offset_i(self,offset):
        self.offset = (self.offset & 0x0000ffff) | ((offset&0xffff)<<16)
        self.u._write_fpga_reg (3,self.offset)

    def set_offset_q(self,offset):
        self.offset = (self.offset & 0xffff0000) | (offset&0xffff)
        self.u._write_fpga_reg (3,self.offset)

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

    def set_gain(self,gain):
        assert gain>=0 and gain<4096
        self.u.write_aux_dac(0,0,int(gain))
        
def main ():
    app = stdgui.stdapp (app_flow_graph, "USRP FFT")
    app.MainLoop ()

if __name__ == '__main__':
    main ()


