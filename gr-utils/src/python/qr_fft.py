#!/usr/bin/env python
#
# Copyright 2004,2005,2007,2008,2009 Free Software Foundation, Inc.
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

from gnuradio.wxgui import forms
from gnuradio import gr, gru
from gnuradio import vrt
from gnuradio import eng_notation
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import stdgui2, fftsink2, waterfallsink2, scopesink2, form, slider
from gnuradio.gr import pubsub
from optparse import OptionParser
import wx
import sys
import numpy
import time

class app_top_block(stdgui2.std_top_block, pubsub.pubsub):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__(self, frame, panel, vbox, argv)
        pubsub.pubsub.__init__(self)
        self.frame = frame
        self.panel = panel
        
        parser = OptionParser(option_class=eng_option)
        #parser.add_option("-e", "--interface", type="string", default="eth0",
        #                  help="select Ethernet interface, default is eth0")
        #parser.add_option("-m", "--mac-addr", type="string", default="",
        #                  help="select USRP by MAC address, default is auto-select")
        #parser.add_option("-A", "--antenna", default=None,
        #                  help="select Rx Antenna (only on RFX-series boards)")
        #parser.add_option("-d", "--decim", type="int", default=16,
        #                  help="set fgpa decimation rate to DECIM [default=%default]")
        #parser.add_option("-f", "--freq", type="eng_float", default=None,
        #                  help="set frequency to FREQ", metavar="FREQ")
        #parser.add_option("-g", "--gain", type="eng_float", default=None,
        #                  help="set gain in dB (default is midpoint)")
        parser.add_option("-W", "--waterfall", action="store_true", default=False,
                          help="Enable waterfall display")
        parser.add_option("-S", "--oscilloscope", action="store_true", default=False,
                          help="Enable oscilloscope display")
	parser.add_option("", "--avg-alpha", type="eng_float", default=1e-1,
			  help="Set fftsink averaging factor, default=[%default]")
	parser.add_option("", "--ref-scale", type="eng_float", default=1.0,
			  help="Set dBFS=0dB input value, default=[%default]")
        parser.add_option("--fft-size", type="int", default=1024,
                          help="Set number of FFT bins [default=%default]")
        parser.add_option("--samples-per-pkt", type="int", default=0,
                          help="Set number of SAMPLES-PER-PKT [default=%default]")
        parser.add_option("", "--ip-addr", type="string", default="192.168.10.2",
                          help="IP address default=[%default]")
        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            sys.exit(1)
	self.options = options
        self.show_debug_info = True
        
        self.u = vrt.quadradio_source_32fc(options.ip_addr,
                                           int(62.5e6), options.samples_per_pkt)
        #self.u.set_decim(options.decim)
        
        #input_rate = self.u.adc_rate() / self.u.decim()
        input_rate = int(120e6/4)
        
        if options.waterfall:
            self.scope = \
              waterfallsink2.waterfall_sink_c (panel, fft_size=1024, sample_rate=input_rate)
        elif options.oscilloscope:
            self.scope = scopesink2.scope_sink_c(panel, sample_rate=input_rate)
        else:
            self.scope = fftsink2.fft_sink_c (panel,
                                              fft_size=options.fft_size,
                                              sample_rate=input_rate, 
					      ref_scale=options.ref_scale,
                                              ref_level=20.0,
                                              y_divs = 12,
					      avg_alpha=options.avg_alpha)

        self.connect(self.u, self.scope)

        self._build_gui(vbox)
	self._setup_events()
	
        # set initial values

        #if options.gain is None:
        #    # if no gain was specified, use the mid-point in dB
        #    g = self.u.gain_range()
        #    options.gain = float(g[0]+g[1])/2

        #if options.freq is None:
        #    # if no freq was specified, use the mid-point
        #    r = self.u.freq_range()
        #    options.freq = float(r[0]+r[1])/2
            
        #self.set_gain(options.gain)

	#if options.antenna is not None:
        #    print "Selecting antenna %s" % (options.antenna,)
        #    self.subdev.select_rx_antenna(options.antenna)

        if self.show_debug_info:
        #    self.myform['decim'].set_value(self.u.decim())
            self.myform['fs@gbe'].set_value(input_rate)
        #    self.myform['dbname'].set_value("0x%04X" % (self.u.daughterboard_id(),)) # FIXME: add text name
            self.myform['baseband'].set_value(0)
            self.myform['ddc'].set_value(0)

        #if not(self.set_freq(options.freq)):
        #    self._set_status_msg("Failed to set initial frequency")

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
        #g = self.u.gain_range()

	# some configurations don't have gain control
	if 0 and g[1] > g[0]:
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
        
        def _form_set_decim(kv):
            return self.set_decim(kv['decim'])

        if not(self.show_debug_info):
            return

        panel = self.panel
        vbox = vbox_arg
        myform = self.myform

        #panel = wx.Panel(self.panel, -1)
        #vbox = wx.BoxSizer(wx.VERTICAL)

        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.Add((5,0), 0)

        myform['decim'] = form.int_field(
            parent=panel, sizer=hbox, label="Decim",
            callback=myform.check_input_and_call(_form_set_decim, self._set_status_msg))

        hbox.Add((5,0), 1)
        myform['fs@gbe'] = form.static_float_field(
            parent=panel, sizer=hbox, label="Fs@GbE")

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
        ##### db control stuff #####
        self.subscribe('cal_div_lo_freq', lambda x: self.u.set_lo_freq(x) and time.sleep(0.01))
        self.subscribe('cal_div_lo_freq', self.u.set_center_freq) #TODO should be combined with set lo freq
        self.subscribe('cal_div_cal_freq', lambda x: self.u.set_cal_freq(x) and time.sleep(0.01))
        self.subscribe('db_ctrl_atten0', self.u.set_attenuation0)
        self.subscribe('db_ctrl_atten1', self.u.set_attenuation1)
        self.subscribe('sys_beaming', self.u.set_beamforming)
        #self.subscribe('db_ctrl_10db', self.u.set_10dB_atten)
        self.subscribe('db_ctrl_adcgain', self.u.set_adc_gain)
        self.subscribe('db_ctrl_diggain', self.u.set_digital_gain)
        self.subscribe('db_ctrl_dcoffset', self.u.set_dc_offset_comp)
        self.subscribe('db_ctrl_bandsel', self.u.set_band_select)
        self.subscribe('db_ctrl_type', self.u.select_rx_antenna)
        self.subscribe('db_test_signal', self.u.set_test_signal)
        self['db_ctrl_bandsel'] = 'A'
        self['cal_div_lo_freq'] = 2.1e9
        self['cal_div_cal_freq'] = 2.102e9
        self['db_ctrl_atten0'] = 0
        self['db_ctrl_atten1'] = 0
        #self['db_ctrl_10db'] = False
        self['db_ctrl_adcgain'] = False
        self['db_ctrl_dcoffset'] = False
        self['db_ctrl_diggain'] = 0.0
        self['db_ctrl_type'] = 'rf'
        self['db_test_signal'] = vrt.VRT_TEST_SIG_NORMAL
	self['sys_beaming'] = [16.7e6, 0, 0, 0]
        #slider and box for freqs
        for key, name in (('cal_div_lo_freq', 'LO Freq'), ('cal_div_cal_freq', 'Cal Freq')):
            hbox = wx.BoxSizer(wx.HORIZONTAL)
            hbox.AddSpacer(10)
            forms.text_box(
                label=name,
                ps=self,
                key=key,
                sizer=hbox,
                parent=panel,
                proportion=0,
                converter=forms.float_converter()
            )
            hbox.AddSpacer(20)
            forms.slider(
                ps=self,
                key=key,
                minimum=0,   #TODO get bounds from cal_div, from vrt...
                maximum=int(3.5e9),
                step_size=int(5e6),
                cast=float,
                sizer=hbox,
                parent=panel,
                proportion=2,
            )
            hbox.AddSpacer(10)
            vbox.Add(hbox, 0, wx.EXPAND)
        ############################################
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.AddSpacer(10)
        #create slider for atten
        atten0_txt_box = forms.static_text(
            label='Attenuation (0)',
            ps=self,
            key='db_ctrl_atten0',
            sizer=hbox,
            parent=panel,
            proportion=0,
            converter=forms.int_converter()
        )
        hbox.AddSpacer(20)
        atten0_slider = forms.slider(
            ps=self,
            key='db_ctrl_atten0',
            minimum=0,
            maximum=31,
            step_size=1,
            cast=int,
            sizer=hbox,
            parent=panel,
            proportion=2,
        )
        hbox.AddSpacer(10)
        #create slider for atten
        forms.static_text(
            label='Attenuation (1)',
            ps=self,
            key='db_ctrl_atten1',
            sizer=hbox,
            parent=panel,
            proportion=0,
            converter=forms.int_converter()
        )
        hbox.AddSpacer(20)
        forms.slider(
            ps=self,
            key='db_ctrl_atten1',
            minimum=0,
            maximum=31,
            step_size=1,
            cast=int,
            sizer=hbox,
            parent=panel,
            proportion=2,
        )
        hbox.AddSpacer(10)
        def update_atten0(*args):
            for form_obj in (atten0_txt_box, atten0_slider): form_obj.Enable(self['db_ctrl_bandsel'] > 'B')
        update_atten0()
        self.subscribe('db_ctrl_bandsel', update_atten0)
        #create checkbox for 10dB att
        #forms.check_box(
        #    label='10dB Attenuation',
        #    ps=self,
        #    key='db_ctrl_10db',
        #    sizer=hbox,
        #    parent=panel,
        #    proportion=1,
        #)
        #hbox.AddSpacer(10)
        vbox.Add(hbox, 0, wx.EXPAND)
        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        hbox2.AddSpacer(10)
        forms.static_text(
            label='ADC Controls',
            ps=self,
            key='db_ctrl_diggain',
            sizer=hbox2,
            parent=panel,
            proportion=0,
            converter=forms.float_converter()
        )
        hbox2.AddSpacer(20)
        #create checkbox for ADC digital gain
        forms.slider(
            #label='ADC Digital Gain',
            ps=self,
            minimum=0,
            maximum=6,
            step_size=0.5,
            key='db_ctrl_diggain',
            sizer=hbox2,
            parent=panel,
            proportion=2,
        )
        hbox2.AddSpacer(10)
        #create checkbox for 3.5dB ADC gain
        forms.check_box(
            label='3.5dB ADC Gain',
            ps=self,
            key='db_ctrl_adcgain',
            sizer=hbox2,
            parent=panel,
            proportion=1,
        )
        hbox2.AddSpacer(10)
        #create checkbox for DC Offset Correction in ADC
        forms.check_box(
            label='DC Offset Correction',
            ps=self,
            key='db_ctrl_dcoffset',
            sizer=hbox2,
            parent=panel,
            proportion=2,
        )
        hbox2.AddSpacer(10)
        vbox.Add(hbox2, 0, wx.EXPAND)
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.AddSpacer(10)
        #create radio buttons for band sel
        forms.radio_buttons(
            label='Band Select',
            ps=self,
            key='db_ctrl_bandsel',
            choices=['A', 'B', 'C', 'D'],
            labels=['A', 'B', 'C', 'D'],
            sizer=hbox,
            parent=panel,
            proportion=0,
        )
        hbox.AddSpacer(10)
        forms.radio_buttons(
            label='RF Input',
            ps=self,
            key='db_ctrl_type',
            choices=['rf', 'cal'],
            labels=['Main RF', 'Calibrator'],
            sizer=hbox,
            parent=panel,
            proportion=0,
        )
        hbox.AddSpacer(10)
        #create radio buttons for band sel
        types = sorted(
            filter(lambda x: x.startswith('VRT_TEST_SIG_'), dir(vrt)),
            lambda x, y: cmp(getattr(vrt, x), getattr(vrt, y)),
        )
        forms.drop_down(
            label='Test Signal',
            ps=self,
            key='db_test_signal',
            choices=map(lambda a: getattr(vrt, a), types),
            labels=types,
            sizer=hbox,
            parent=panel,
            proportion=0,
        )
        hbox.AddSpacer(10)
        #create radio buttons for type
        forms.drop_down(
            label='Beamformer',
            ps=self,
            key='sys_beaming',
            choices=[[16.7e6, 0, 0, 0], [0, 16.7e6, 0, 0], [0, 0, 16.7e6, 0], [0, 0, 0, 16.7e6], [4.19e6]*4],
            labels=['Ant0', 'Ant1', 'Ant2', 'Ant3', 'Equal Gain'],
            sizer=hbox,
            parent=panel,
            proportion=0,
        )
        hbox.AddSpacer(10)
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
        return True
    
        r = self.u.set_center_freq(target_freq)
        
        if r:
            self.myform['freq'].set_value(target_freq)     # update displayed value
            if self.show_debug_info:
                self.myform['baseband'].set_value(r.baseband_freq)
                self.myform['ddc'].set_value(r.dxc_freq)
	    if not self.options.oscilloscope:
		self.scope.win.set_baseband_freq(target_freq)
    	    return True

        return False

    def set_gain(self, gain):
        return True
    
	if self.myform.has_key('gain'):
    	    self.myform['gain'].set_value(gain)     # update displayed value
        self.u.set_gain(gain)

    def set_decim(self, decim):
        return True
    
        ok = self.u.set_decim(decim)
        if not ok:
            print "set_decim failed"
        #input_rate = self.u.adc_rate() / self.u.decim()
        input_rate = 120e6/4
        self.scope.set_sample_rate(input_rate)
        if self.show_debug_info:  # update displayed values
            self.myform['decim'].set_value(self.u.decim())
            self.myform['fs@gbe'].set_value(input_rate)
        return ok

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
    app = stdgui2.stdapp(app_top_block, "QuadRadio FFT", nstatus=1)
    app.MainLoop()

if __name__ == '__main__':
    main ()
