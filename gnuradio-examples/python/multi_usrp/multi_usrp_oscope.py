#!/usr/bin/env python
#
# Copyright 2004,2005,2006 Free Software Foundation, Inc.
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

# print "Loading revised usrp_oscope with additional options for scopesink..."

from gnuradio import gr, gru
from gnuradio import usrp
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui, fftsink, waterfallsink, scopesink, form, slider
from optparse import OptionParser
import wx
import sys

import time
from gnuradio import usrp_multi


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


class app_flow_graph(stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__(self)

        self.frame = frame
        self.panel = panel
        
        parser = OptionParser(option_class=eng_option)
        parser.add_option("-R", "--rx-subdev-spec", type="subdev", default=None,
                          help="select USRP Rx side A or B (default=first one with a daughterboard)")
        parser.add_option("-d", "--decim", type="int", default=128,
                          help="set fgpa decimation rate to DECIM [default=%default]")
        parser.add_option("-f", "--freq", type="eng_float", default=None,
                          help="set frequency to FREQ", metavar="FREQ")
        parser.add_option("-g", "--gain", type="eng_float", default=None,
                          help="set gain in dB (default is midpoint)")
        #align interval is default not yet enabled in gr.align_on_samplenumbers_ss
        #parser.add_option("-a", "--align_interval", type="int", default=-1,
        #                  help="Align master and slave every so much samples.")

        # width 8 does not work yet with multi_usrp because it interferes with the 32 bit samplecounter
        #parser.add_option("-8", "--width-8", action="store_true", default=False,
        #                  help="Enable 8-bit samples across USB")
        parser.add_option("-m", "--mux", type="intx", default=None,
                          help="set fpga FR_RX_MUX register to MUX")
        parser.add_option("-n", "--frame-decim", type="int", default=1,
                          help="set oscope frame decimation factor to n [default=1]")
        parser.add_option("-N", "--nchan", type="int", default=2,
                          help="set nchannels to NCHAN")
        parser.add_option("-q", "--show-q", action="store_true", default=False,
                          help="show the q value of the complex samples")
        parser.add_option("-s", "--show-counters", action="store_true", default=False,
                          help="show the counters")
        parser.add_option("-v", "--v-scale", type="eng_float", default=1000,
                          help="set oscope initial V/div to SCALE [default=%default]")
        parser.add_option("-t", "--t-scale", type="eng_float", default=49e-6,
                          help="set oscope initial s/div to SCALE [default=50us]")
        parser.add_option("-x", "--master-serialno", type="string", default=None, 
                          help="Serial_no of the usrp which should be the MASTER (default= select any)")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)

        self.show_debug_info = True
        
        # build the graph

        #self.u = usrp.source_c(which=options.which_usrp,decim_rate=options.decim)
        if (options.mux is None) | (4==options.nchan):
           init_mux=None #use default mux which is 0x10321032
        else:
           init_mux=options.mux

        init_gain=0.0
        init_freq=0.0
        init_align_interval=-1

        self.multi=usrp_multi.multi_source_align( self,   options.master_serialno, options.decim,
                                                 options.nchan, init_gain, init_freq, init_mux, init_align_interval)
        self.um=self.multi.get_master_usrp()
        self.us=self.multi.get_slave_usrp()

        if options.rx_subdev_spec is None:
            options.rx_subdev_spec = pick_subdevice(self.um)
        if (options.mux==None) and (options.nchan!=4):
          mux=usrp.determine_rx_mux_value(self.um, options.rx_subdev_spec)
          mux= (mux<<8 & 0xffffffff) | (mux & 0xff)
          self.um.set_mux(mux)
          self.us.set_mux(mux)

        # width 8 does not work yet with multi_usrp because it interferes with the 32 bit samplecounter
        #if options.width_8:
        #    width = 8
        #    shift = 8
        #    format = self.um.make_format(width, shift)
        #    r = self.um.set_format(format)
        #    r = self.us.set_format(format)
            
        # determine the daughterboard subdevice of the first channel we're using
        self.subdevm = usrp.selected_subdev(self.um, options.rx_subdev_spec)
        self.subdevs = usrp.selected_subdev(self.us, options.rx_subdev_spec)

        input_rate = self.um.adc_freq() / self.um.decim_rate()

        self.scope = scopesink.scope_sink_f(self, panel, sample_rate=input_rate,
                                            frame_decim=options.frame_decim,
                                            v_scale=options.v_scale,
                                            t_scale=options.t_scale)
        self.sink_count=0
        self.add_to_scope((self.multi.get_master_source_c(),1),options.show_q)
        self.add_to_scope((self.multi.get_slave_source_c(),1),options.show_q)
        if 4==options.nchan:
          self.add_to_scope((self.multi.get_master_source_c(),2),options.show_q)
          self.add_to_scope((self.multi.get_slave_source_c(),2),options.show_q)

        if options.show_counters:
          self.add_to_scope((self.multi.get_master_source_c(),0),options.show_q)
          self.add_to_scope((self.multi.get_slave_source_c(),0),options.show_q)

        self._build_gui(vbox)

        # set initial values

        if options.gain is None:
            # if no gain was specified, use the mid-point in dB
            g = self.subdevm.gain_range()
            options.gain = float(g[0]+g[1])/2

        if options.freq is None:
            # if no freq was specified, use the mid-point
            r = self.subdevm.freq_range()
            options.freq = float(r[0]+r[1])/2

        self.set_gain(options.gain)

        if self.show_debug_info:
            self.myform['decim'].set_value(self.um.decim_rate())
            self.myform['fs@usb'].set_value(self.um.adc_freq() / self.um.decim_rate())
            self.myform['dbname'].set_value(self.subdevm.name())
            self.myform['baseband'].set_value(0)
            self.myform['ddc'].set_value(0)
                        
        if not(self.set_freq(options.freq)):
            self._set_status_msg("Failed to set initial frequency")

        self.multi.print_db_info()
        self.unsynced=True
        frame.Bind(wx.EVT_IDLE, self.onIdle)

    def add_to_scope(self,source_c,show_q):
        c2f= gr.complex_to_float ()
        self.connect(source_c, c2f)
        self.connect((c2f,0), (self.scope,self.sink_count))
        self.sink_count=self.sink_count+1
        if show_q:
          self.connect((c2f,1), (self.scope,self.sink_count))
          self.sink_count=self.sink_count+1
        
        
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
            callback=myform.check_input_and_call(_form_set_freq, self._set_status_msg))

        hbox.Add((5,0), 0, 0)
        g = self.subdevm.gain_range()
        myform['gain'] = form.slider_field(parent=self.panel, sizer=hbox, label="Gain",
                                           weight=3,
                                           min=int(g[0]), max=int(g[1]),
                                           callback=self.set_gain)

        hbox.Add((5,0), 0, 0)
        buttonSync = form.button_with_callback(parent=self.panel, label='sync',callback=self.sync_usrps)
        hbox.Add(buttonSync,0,wx.EXPAND)

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


    def set_freq(self, target_freq, sync=True):
        """
        Set the center frequency we're interested in for all channels,
        on all rx daughterboards on MASTER and SLAVE.

        @param target_freq: frequency in Hz
        @param sync: sync the usrps after setting the freqs (this will clear any phase differences in the DDCS)
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        result,r1,r2,r3,r4 = self.multi.tune_all_rx(target_freq)
        if sync:
          self.sync_usrps() #sync master and slave and clear any DDC phase differences
        if r1:
            self.myform['freq'].set_value(target_freq)     # update displayed value
            if self.show_debug_info:
                self.myform['baseband'].set_value(r1.baseband_freq)
                self.myform['ddc'].set_value(r1.dxc_freq)
        return result

    def set_freq_chan0(self, target_freq, sync=True):
        """
        Set the center frequency we're interested in for rx chan 0 only on MASTER and SLAVE.

        @param target_freq: frequency in Hz
        @param sync: sync the usrps after setting the freqs (this will clear any phase differences in the DDCS)
        @rypte: bool

        Tuning is a two step process.  First we ask the front-end to
        tune as close to the desired frequency as it can.  Then we use
        the result of that operation and our target_frequency to
        determine the value for the digital down converter.
        """
        rm = usrp.tune(self.um, 0, self.subdevm, target_freq)
        rs = usrp.tune(self.us, 0, self.subdevs, target_freq)
        r=rm
        if sync:
          self.sync_usrps()  #sync master and slave and clear any DDC phase differences
        if r:
            self.myform['freq'].set_value(target_freq)     # update displayed value
            if self.show_debug_info:
                self.myform['baseband'].set_value(r.baseband_freq)
                self.myform['ddc'].set_value(r.dxc_freq)
            return True

        return False

    def set_gain(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.multi.set_gain_all_rx(gain)

    def set_gain_chan0(self, gain):
        self.myform['gain'].set_value(gain)     # update displayed value
        self.subdevm.set_gain(gain)
        self.subdevs.set_gain(gain)

    def onIdle(self,evt):
        if self.unsynced:
          time.sleep(0.5)
          self.unsynced=True
          self.multi.sync()
          self.unsynced=False
          #print 'synced'

    def sync_usrps(self):
        self.multi.sync()

def main ():
    app = stdgui.stdapp(app_flow_graph, "MULTI_USRP O'scope", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
