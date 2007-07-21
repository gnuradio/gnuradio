#!/usr/bin/env python
#
# This is mrfm_fft_sos.py
# Modification of Matt's mrfm_fft.py that reads filter coefs from file
#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, fftsink, waterfallsink, scopesink, form, slider
from optparse import OptionParser
import wx
import sys
import mrfm


def pick_subdevice(u):
    """
    The user didn't specify a subdevice on the command line.
    If there's a daughterboard on A, select A.
    If there's a daughterboard on B, select B.
    Otherwise, select A.
    """
    if u.db[0][0].dbid() >= 0:       # dbid is < 0 if there's no d'board or a problem
        return (0, 0)
    if u.db[1][0].dbid() >= 0:
        return (1, 0)
    return (0, 0)

def read_ints(filename):
    try:
        f = open(filename)
        ints = [ int(i) for i in f.read().split() ]
        f.close()
        return ints
    except:
        return []

class app_flow_graph(stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__(self)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=first one with a daughterboard)")
        parser.add_option("-d", "--decim", type="int", default=16,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-W", "--waterfall", action="store_true", default=False,
                          help="Enable waterfall display")
        parser.add_option("-8", "--width-8", action="store_true", default=False,
                          help="Enable 8-bit samples across USB")
        parser.add_option("-S", "--oscilloscope", action="store_true", default=False,
                          help="Enable oscilloscope display")
        parser.add_option("-F", "--filename", default=None,
                          help="Name of file with filter coefficients")
        parser.add_option("-C", "--cfilename", default=None,
                          help="Name of file with compensator coefficients")
        parser.add_option("-B", "--bitstream", default="mrfm.rbf",
                          help="Name of FPGA Bitstream file (.rbf)")
        parser.add_option("-n", "--frame-decim", type="int", default=20,
                          help="set oscope frame decimation factor to n [default=12]")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        self.show_debug_info = True

        # default filter coefs
        b00 = b01 = 16384  
        b10 = b20 = a10 = a20 = b11 = b21 = a11 = a21 = 0

        ba = read_ints(options.filename)
        if len(ba) >= 6:
            b00 = ba[0]; b10 = ba[1]; b20 = ba[2]; a10 = ba[4]; a20 = ba[5]
        if len(ba) >= 12:
            b01 = ba[6]; b11 = ba[7]; b21 = ba[8]; a11 = ba[10]; a21=ba[11]
        print b00, b10, b20, a10, a20, b01, b11, b21, a11, a21 

        # default compensator coefficients
        c11 = c22 = 1
        c12 = c21 = cscale = 0

        cs = read_ints(options.cfilename)
        if len(cs) >= 5:
            c11 = cs[0]; c12 = cs[1]; c21 = cs[2]; c22 = cs[3]; cscale = cs[4]
        print c11, c12, c21, c22, cscale
     
        # build the graph
        self.u = mrfm.source_c(options.bitstream)

        self.u.set_decim_rate(options.decim)
        self.u.set_center_freq(options.freq)

        frac_bits = 14
        self.u.set_coeffs(frac_bits,b20,b10,b00,a20,a10,b21,b11,b01,a21,a11)

        self.u.set_compensator(c11,c12,c21,c22,cscale)

        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = pick_subdevice(self.u)
        self.u.set_mux(usrp.determine_rx_mux_value(self.u, options.rx_subdev_spec))

        if options.width_8:
            width = 8
            shift = 8
            format = self.u.make_format(width, shift)
            print "format =", hex(format)
            r = self.u.set_format(format)
            print "set_format =", r
            
        # determine the daughterboard subdevice we're using
        self.subdev = usrp.selected_subdev(self.u, options.rx_subdev_spec)

        #input_rate = self.u.adc_freq() / self.u.decim_rate()
        input_rate = self.u.adc_freq() / options.decim

        # fft_rate = 15
        fft_rate = 5
                
        self.deint = gr.deinterleave(gr.sizeof_gr_complex)
        self.connect(self.u,self.deint)
        
        if options.waterfall:
            self.scope1=waterfallsink.waterfall_sink_c (self, panel, fft_size=1024, sample_rate=input_rate,
                                                        fft_rate=fft_rate)
            self.scope2=waterfallsink.waterfall_sink_c (self, panel, fft_size=1024, sample_rate=input_rate,
                                                        fft_rate=fft_rate)

        elif options.oscilloscope:
            self.scope1 = scopesink.scope_sink_c(self, panel, sample_rate=input_rate,frame_decim=options.frame_decim) # added option JPJ 4/21/2006
            self.scope2 = scopesink.scope_sink_c(self, panel, sample_rate=input_rate,frame_decim=options.frame_decim) 

        else:
            self.scope1 = fftsink.fft_sink_c (self, panel, fft_size=1024, sample_rate=input_rate,
                                             fft_rate=fft_rate)
            self.scope2 = fftsink.fft_sink_c (self, panel, fft_size=1024, sample_rate=input_rate,
                                             fft_rate=fft_rate)

        # Show I, I' on top scope panel, Q, Q' on bottom
        #self.fin = gr.complex_to_float()
        #self.fout = gr.complex_to_float()

        #self.connect((self.deint,0), self.fin)
        #self.connect((self.deint,1), self.fout)

        #self.ii = gr.float_to_complex()
        #self.qq = gr.float_to_complex()
        
        #self.connect((self.fin,0), (self.ii,0))
        #self.connect((self.fout,0), (self.ii,1))
        #self.connect((self.fin,1), (self.qq,0))
        #self.connect((self.fout,1), (self.qq,1))
        
        #self.connect(self.ii, self.scope1)
        #self.connect(self.qq, self.scope2)

        self.connect ((self.deint,0),self.scope1)
        self.connect ((self.deint,1),self.scope2)

        self._build_gui(vbox)

        # set initial values

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdev.gain_range()
            options.gain = float(g[0]+g[1])/2

        if options.freq is None:
            # if no freq was specified, use the mid-point
            r = self.subdev.freq_range()
            options.freq = float(r[0]+r[1])/2

        self.set_gain(options.gain)

        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

        if self.show_debug_info:
            self.myform['decim'].set_value(self.u.decim_rate())
            self.myform['fs@usb'].set_value(self.u.adc_freq() / self.u.decim_rate())
            self.myform['dbname'].set_value(self.subdev.name())
                        

    def _set_status_msg(self, msg):
        self.frame.GetStatusBar().SetStatusText(msg, 0)

    def _build_gui(self, vbox):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])
            
        vbox.Add(self.scope1.win, 10, wx.EXPAND)
        vbox.Add(self.scope2.win, 10, wx.EXPAND)
        
        # add control area at the bottom
        self.myform = myform = form.form()
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0, 0)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=hbox, label="Center freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

        hbox.Add((5,0), 0, 0)
        g = self.subdev.gain_range()
        myform['gain'] = form.slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                           weight=3,
                                           min=int(g[0]), max=int(g[1]),
                                           callback=self.set_gain)

        hbox.Add((5,0), 0, 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        self._build_subpanel(vbox)

    def _build_subpanel(self, vbox_arg):
        # build a secondary information panel (sometimes hidden)

        # FIXME figure out how to have this be a subpanel that is always
        # created, but has its visibility controlled by foo.Show(True/False)
        
        if not(self.show_debug_info):
            return

        panel = self.panel
        vbox = vbox_arg
        myform = self.myform

        #panel = wx.Panel(self.panel, -1)
        #vbox = wx.BoxSizer(wx.VERTICAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)
        myform['decim'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Decim")

        hbox.Add((5,0), 1)
        myform['fs@usb'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Fs@USB")

        hbox.Add((5,0), 1)
        myform['dbname'] = form.static_text_field(
            parent=panel, sizer=hbox)

        hbox.Add((5,0), 1)
        myform['baseband'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Analog BB")

        hbox.Add((5,0), 1)
        myform['ddc'] = form.static_float_field(
            parent=panel, sizer=hbox, label="DDC")

        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)

        
        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        r = self.u.tune(0, self.subdev, target_freq)
        
        if r:
            self.myform['freq'].set_value(target_freq)     # update displayed value
            if self.show_debug_info:
                self.myform['baseband'].set_value(r.baseband_freq)
                self.myform['ddc'].set_value(r.dxc_freq)
            return True

        return False

    def set_gain(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.subdev.set_gain(gain)


def main ():
    app = stdgui.stdapp(app_flow_graph, "USRP FFT", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
