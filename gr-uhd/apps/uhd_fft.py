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

from gnuradio import gr
from gnuradio import uhd
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from optparse import OptionParser

import sys
import numpy

try:
    from gnuradio.wxgui import stdgui2, form, slider
    from gnuradio.wxgui import fftsink2, waterfallsink2, scopesink2
    import wx
except ImportError:
    sys.stderr.write("Error importing GNU Radio's wxgui. Please make sure gr-wxgui is installed.\n")
    sys.exit(1)

class app_top_block(stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__(self, frame, panel, vbox, argv)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-a", "--args", type="string", default="",
                          help="UHD device address args , [default=%default]")
        parser.add_option("", "--spec", type="string", default=None,
                          help="Subdevice of UHD device where appropriate")
        parser.add_option("-A", "--antenna", type="string", default=None,
                          help="select Rx Antenna where appropriate")
        parser.add_option("-s", "--samp-rate", type="eng_float", default=1e6,
                          help="set sample rate (bandwidth) [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        parser.add_option("-W", "--waterfall", action="store_true", default=False,
                          help="Enable waterfall display")
        parser.add_option("-S", "--oscilloscope", action="store_true", default=False,
                          help="Enable oscilloscope display")
        parser.add_option("", "--avg-alpha", type="eng_float", default=1e-1,
                          help="Set fftsink averaging factor, default=[%default]")
        parser.add_option ("", "--averaging", action="store_true", default=False,
                           help="Enable fftsink averaging, default=[%default]")
        parser.add_option("", "--ref-scale", type="eng_float", default=1.0,
                          help="Set dBFS=0dB input value, default=[%default]")
        parser.add_option("", "--fft-size", type="int", default=1024,
                          help="Set number of FFT bins [default=%default]")
        parser.add_option("", "--fft-rate", type="int", default=30,
                          help="Set FFT update rate, [default=%default]")
        parser.add_option("", "--wire-format", type="string", default="sc16",
                          help="Set wire format from USRP [default=%default]")
        parser.add_option("", "--scalar", type="int", default=1024,
                          help="Set scalar multiplier value sc8 wire format [default=%default]")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
        self.options = options
        self.show_debug_info = True
        
        scalar="scalar="+str(options.scalar)
        self.u = uhd.usrp_source(device_addr=options.args,
                                 stream_args=uhd.stream_args(cpu_format='fc32',
                                 otw_format=options.wire_format, args=scalar))

        # Set the subdevice spec
        if(options.spec):
            self.u.set_subdev_spec(options.spec, 0)

        # Set the antenna
        if(options.antenna):
            self.u.set_antenna(options.antenna, 0)

        self.u.set_samp_rate(options.samp_rate)
        input_rate = self.u.get_samp_rate()
        
        if options.waterfall:
            self.scope = \
              waterfallsink2.waterfall_sink_c (panel, fft_size=1024,
                                               sample_rate=input_rate)
            self.frame.SetMinSize((800, 420))
        elif options.oscilloscope:
            self.scope = scopesink2.scope_sink_c(panel, sample_rate=input_rate)
            self.frame.SetMinSize((800, 600))
        else:
            self.scope = fftsink2.fft_sink_c (panel,
                                              fft_size=options.fft_size,
                                              sample_rate=input_rate, 
                          ref_scale=options.ref_scale,
                                              ref_level=20.0,
                                              y_divs = 12,
                                              average=options.averaging,
                          avg_alpha=options.avg_alpha,
                                              fft_rate=options.fft_rate)
            self.frame.SetMinSize((800, 420))

        self.connect(self.u, self.scope)

        self._build_gui(vbox)
        self._setup_events()

    
        # set initial values

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.u.get_gain_range()
            options.gain = float(g.start()+g.stop())/2

        if options.freq is None:
            # if no freq was specified, use the mid-point
            r = self.u.get_freq_range()
            options.freq = float(r.start()+r.stop())/2
            
        self.set_gain(options.gain)

        if self.show_debug_info:
            self.myform['samprate'].set_value(self.u.get_samp_rate())
            self.myform['rffreq'].set_value(0)
            self.myform['dspfreq'].set_value(0)

        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

    def _set_status_msg(self, msg):
        self.frame.GetStatusBar().SetStatusText(msg, 0)

    def _build_gui(self, vbox):

        def _form_set_freq(kv):
            return self.set_freq(kv['freq'])
            
        vbox.Add(self.scope.win, 10, wx.EXPAND)
        
        # add control area at the bottom
        self.myform = myform = form.form()
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0, 0)
        myform['freq'] = form.float_field(
            parent=self.panel, sizer=hbox, label="Center freq", weight=1,
            callback=myform.check_input_and_call(_form_set_freq,
                                                 self._set_status_msg))

        hbox.Add((5,0), 0, 0)
        g = self.u.get_gain_range()

        # some configurations don't have gain control
        if g.stop() > g.start():
            myform['gain'] = form.slider_field(parent=self.panel,
                                                   sizer=hbox, label="Gain",
                                                   weight=3,
                                                   min=int(g.start()), max=int(g.stop()),
                                                   callback=self.set_gain)

            hbox.Add((5,0), 0, 0)
            vbox.Add(hbox, 0, wx.EXPAND)

            self._build_subpanel(vbox)

    def _build_subpanel(self, vbox_arg):
        # build a secondary information panel (sometimes hidden)

        # FIXME figure out how to have this be a subpanel that is always
        # created, but has its visibility controlled by foo.Show(True/False)
        
        def _form_set_samp_rate(kv):
            return self.set_samp_rate(kv['samprate'])

        if not(self.show_debug_info):
            return

        panel = self.panel
        vbox = vbox_arg
        myform = self.myform

        hbox = wx.BoxSizer(wx.HORIZONTAL)

        hbox.Add((5,0), 0)
        myform['samprate'] = form.float_field(
            parent=panel, sizer=hbox, label="Sample Rate",
            callback=myform.check_input_and_call(_form_set_samp_rate,
                                                 self._set_status_msg))

        hbox.Add((5,0), 1)
        myform['rffreq'] = form.static_float_field(
            parent=panel, sizer=hbox, label="RF Freq.")

        hbox.Add((5,0), 1)
        myform['dspfreq'] = form.static_float_field(
            parent=panel, sizer=hbox, label="DSP Freq.")

        hbox.Add((5,0), 0)
        vbox.Add(hbox, 0, wx.EXPAND)
        
    def set_freq(self, target_freq):
        """
        Set the center frequency we're interested in.

        @param target_freq: frequency in Hz
        @rypte: bool
        """
        r = self.u.set_center_freq(target_freq, 0)

        if r:
            self.myform['freq'].set_value(self.u.get_center_freq())
            self.myform['rffreq'].set_value(r.actual_rf_freq)
            self.myform['dspfreq'].set_value(r.actual_dsp_freq)
            if not self.options.oscilloscope:
                self.scope.set_baseband_freq(target_freq)
            return True

        return False

    def set_gain(self, gain):
        if self.myform.has_key('gain'):
            self.myform['gain'].set_value(gain)     # update displayed value
        self.u.set_gain(gain, 0)

    def set_samp_rate(self, samp_rate):
        ok = self.u.set_samp_rate(samp_rate)
        input_rate = self.u.get_samp_rate()
        self.scope.set_sample_rate(input_rate)
        if self.show_debug_info:  # update displayed values
            self.myform['samprate'].set_value(self.u.get_samp_rate())

        # uhd set_samp_rate never fails; always falls back to closest requested.
        return True  

    def _setup_events(self):
        if not self.options.waterfall and not self.options.oscilloscope:
            self.scope.win.Bind(wx.EVT_LEFT_DCLICK, self.evt_left_dclick)
        
    def evt_left_dclick(self, event):
        (ux, uy) = self.scope.win.GetXY(event)
        if event.CmdDown():
            # Re-center on maximum power
            points = self.scope.win._points
            if self.scope.win.peak_hold:
                if self.scope.win.peak_vals is not None:
                    ind = numpy.argmax(self.scope.win.peak_vals)
                else:
                    ind = int(points.shape()[0]/2)
            else:
                ind = numpy.argmax(points[:,1])
                
            (freq, pwr) = points[ind]
            target_freq = freq/self.scope.win._scale_factor
            print ind, freq, pwr
            self.set_freq(target_freq)            
        else:
            # Re-center on clicked frequency
            target_freq = ux/self.scope.win._scale_factor
            self.set_freq(target_freq)
        
    
def main ():
    app = stdgui2.stdapp(app_top_block, "UHD FFT", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
