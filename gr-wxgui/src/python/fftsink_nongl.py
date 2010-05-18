#!/usr/bin/env python
#
# Copyright 2003,2004,2005,2006,2007,2009,2010 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, window
from gnuradio.wxgui import stdgui2
import wx
import plot
import numpy
import math    

DIV_LEVELS = (1, 2, 5, 10, 20)

default_fftsink_size = (640,240)
default_fft_rate = gr.prefs().get_long('wxgui', 'fft_rate', 15)

class fft_sink_base(object):
    def __init__(self, input_is_real=False, baseband_freq=0, y_per_div=10, 
                 y_divs=8, ref_level=50,
                 sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate,
                 average=False, avg_alpha=None, title='', peak_hold=False,use_persistence=False,persist_alpha=0.2):

        # initialize common attributes
        self.baseband_freq = baseband_freq
        self.y_per_div=y_per_div
        self.y_divs = y_divs
        self.ref_level = ref_level
        self.sample_rate = sample_rate
        self.fft_size = fft_size
        self.fft_rate = fft_rate
        self.average = average
        if avg_alpha is None:
            self.avg_alpha = 2.0 / fft_rate
        else:
            self.avg_alpha = avg_alpha
        self.use_persistence = use_persistence
        self.persist_alpha = persist_alpha

        self.title = title
        self.peak_hold = peak_hold
        self.input_is_real = input_is_real
        self.msgq = gr.msg_queue(2)         # queue that holds a maximum of 2 messages

    def set_y_per_div(self, y_per_div):
        self.y_per_div = y_per_div

    def set_ref_level(self, ref_level):
        self.ref_level = ref_level

    def set_average(self, average):
        self.average = average
        if average:
            self.avg.set_taps(self.avg_alpha)
        else:
            self.avg.set_taps(1.0)
        self.win.peak_vals = None
        
    def set_peak_hold(self, enable):
        self.peak_hold = enable
        self.win.set_peak_hold(enable)

    def set_use_persistence(self, enable):
        self.use_persistence = enable
        self.win.set_use_persistence(enable)

    def set_persist_alpha(self, persist_alpha):
        self.persist_alpha = persist_alpha
        self.win.set_persist_alpha(persist_alpha)

    def set_avg_alpha(self, avg_alpha):
        self.avg_alpha = avg_alpha

    def set_baseband_freq(self, baseband_freq):
        self.baseband_freq = baseband_freq

    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
        self._set_n()

    def _set_n(self):
        self.one_in_n.set_n(max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        

class fft_sink_f(gr.hier_block2, fft_sink_base):
    def __init__(self, parent, baseband_freq=0, ref_scale=2.0,
                 y_per_div=10, y_divs=8, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, peak_hold=False, use_persistence=False,persist_alpha=0.2, **kwargs):

        gr.hier_block2.__init__(self, "fft_sink_f",
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(0,0,0))

        fft_sink_base.__init__(self, input_is_real=True, baseband_freq=baseband_freq,
                               y_per_div=y_per_div, y_divs=y_divs, ref_level=ref_level,
                               sample_rate=sample_rate, fft_size=fft_size,
                               fft_rate=fft_rate,
                               average=average, avg_alpha=avg_alpha, title=title,
                               peak_hold=peak_hold,use_persistence=use_persistence,persist_alpha=persist_alpha)
                               
        self.s2p = gr.stream_to_vector(gr.sizeof_float, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_float * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        
        mywindow = window.blackmanharris(self.fft_size)
        self.fft = gr.fft_vfc(self.fft_size, True, mywindow)
        power = 0
        for tap in mywindow:
            power += tap*tap
            
        self.c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)

        # FIXME  We need to add 3dB to all bins but the DC bin
        self.log = gr.nlog10_ff(20, self.fft_size,
                               -10*math.log10(self.fft_size)                # Adjust for number of bins
                               -10*math.log10(power/self.fft_size)        # Adjust for windowing loss
                               -20*math.log10(ref_scale/2))                # Adjust for reference scale
                               
        self.sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)
        self.connect(self, self.s2p, self.one_in_n, self.fft, self.c2mag, self.avg, self.log, self.sink)

        self.win = fft_window(self, parent, size=size)
        self.set_average(self.average)
        self.set_peak_hold(self.peak_hold)
        self.set_use_persistence(self.use_persistence)
        self.set_persist_alpha(self.persist_alpha)

class fft_sink_c(gr.hier_block2, fft_sink_base):
    def __init__(self, parent, baseband_freq=0, ref_scale=2.0,
                 y_per_div=10, y_divs=8, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, peak_hold=False, use_persistence=False,persist_alpha=0.2, **kwargs):

        gr.hier_block2.__init__(self, "fft_sink_c",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(0,0,0))

        fft_sink_base.__init__(self, input_is_real=False, baseband_freq=baseband_freq,
                               y_per_div=y_per_div, y_divs=y_divs, ref_level=ref_level,
                               sample_rate=sample_rate, fft_size=fft_size,
                               fft_rate=fft_rate,
                               average=average, avg_alpha=avg_alpha, title=title,
                               peak_hold=peak_hold, use_persistence=use_persistence,persist_alpha=persist_alpha)

        self.s2p = gr.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_gr_complex * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        
        mywindow = window.blackmanharris(self.fft_size)
        self.fft = gr.fft_vcc(self.fft_size, True, mywindow)
        power = 0
        for tap in mywindow:
            power += tap*tap
            
        self.c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)

        # FIXME  We need to add 3dB to all bins but the DC bin
        self.log = gr.nlog10_ff(20, self.fft_size,
                                -10*math.log10(self.fft_size)                # Adjust for number of bins
                                -10*math.log10(power/self.fft_size)        # Adjust for windowing loss
                                -20*math.log10(ref_scale/2))                # Adjust for reference scale
                                
        self.sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)
        self.connect(self, self.s2p, self.one_in_n, self.fft, self.c2mag, self.avg, self.log, self.sink)

        self.win = fft_window(self, parent, size=size)
        self.set_average(self.average)
        self.set_use_persistence(self.use_persistence)
        self.set_persist_alpha(self.persist_alpha)
        self.set_peak_hold(self.peak_hold)


# ------------------------------------------------------------------------

myDATA_EVENT = wx.NewEventType()
EVT_DATA_EVENT = wx.PyEventBinder (myDATA_EVENT, 0)


class DataEvent(wx.PyEvent):
    def __init__(self, data):
        wx.PyEvent.__init__(self)
        self.SetEventType (myDATA_EVENT)
        self.data = data

    def Clone (self): 
        self.__class__ (self.GetId())


class input_watcher (gru.msgq_runner):
    def __init__ (self, msgq, fft_size, event_receiver, **kwds):
        self.fft_size = fft_size
        self.event_receiver = event_receiver
        gru.msgq_runner.__init__(self, msgq, self.handle_msg)

    def handle_msg(self, msg):
        itemsize = int(msg.arg1())
        nitems = int(msg.arg2())

        s = msg.to_string() # get the body of the msg as a string

        # There may be more than one FFT frame in the message.
        # If so, we take only the last one
        if nitems > 1:
            start = itemsize * (nitems - 1)
            s = s[start:start+itemsize]

        complex_data = numpy.fromstring (s, numpy.float32)
        de = DataEvent (complex_data)
        wx.PostEvent (self.event_receiver, de)
        del de

class control_panel(wx.Panel):
    
    class LabelText(wx.StaticText):    
        def __init__(self, window, label):
            wx.StaticText.__init__(self, window, -1, label)
            font = self.GetFont()
            font.SetWeight(wx.FONTWEIGHT_BOLD)
            font.SetUnderlined(True)
            self.SetFont(font)
    
    def __init__(self, parent):
        self.parent = parent
        wx.Panel.__init__(self, parent, -1, style=wx.SIMPLE_BORDER)    
        control_box = wx.BoxSizer(wx.VERTICAL)
        
        #checkboxes for average and peak hold
        control_box.AddStretchSpacer()
        control_box.Add(self.LabelText(self, 'Options'), 0, wx.ALIGN_CENTER)
        self.average_check_box = wx.CheckBox(parent=self, style=wx.CHK_2STATE, label="Average")
        self.average_check_box.Bind(wx.EVT_CHECKBOX, parent.on_average)
        control_box.Add(self.average_check_box, 0, wx.EXPAND)
        self.use_persistence_check_box = wx.CheckBox(parent=self, style=wx.CHK_2STATE, label="Persistence")
        self.use_persistence_check_box.Bind(wx.EVT_CHECKBOX, parent.on_use_persistence)
        control_box.Add(self.use_persistence_check_box, 0, wx.EXPAND)
        self.peak_hold_check_box = wx.CheckBox(parent=self, style=wx.CHK_2STATE, label="Peak Hold")
        self.peak_hold_check_box.Bind(wx.EVT_CHECKBOX, parent.on_peak_hold) 
        control_box.Add(self.peak_hold_check_box, 0, wx.EXPAND)
       
        #radio buttons for div size
        control_box.AddStretchSpacer()
        control_box.Add(self.LabelText(self, 'Set dB/div'), 0, wx.ALIGN_CENTER)
        radio_box = wx.BoxSizer(wx.VERTICAL)
        self.radio_buttons = list()
        for y_per_div in DIV_LEVELS:
            radio_button = wx.RadioButton(self, -1, "%d dB/div"%y_per_div)
            radio_button.Bind(wx.EVT_RADIOBUTTON, self.on_radio_button_change)
            self.radio_buttons.append(radio_button)
            radio_box.Add(radio_button, 0, wx.ALIGN_LEFT)
        control_box.Add(radio_box, 0, wx.EXPAND)
        
        #ref lvl buttons
        control_box.AddStretchSpacer()
        control_box.Add(self.LabelText(self, 'Adj Ref Lvl'), 0, wx.ALIGN_CENTER)
        control_box.AddSpacer(2)
        button_box = wx.BoxSizer(wx.HORIZONTAL)        
        self.ref_plus_button = wx.Button(self, -1, '+', style=wx.BU_EXACTFIT)
        self.ref_plus_button.Bind(wx.EVT_BUTTON, parent.on_incr_ref_level)
        button_box.Add(self.ref_plus_button, 0, wx.ALIGN_CENTER)
        self.ref_minus_button = wx.Button(self, -1, ' - ', style=wx.BU_EXACTFIT)
        self.ref_minus_button.Bind(wx.EVT_BUTTON, parent.on_decr_ref_level)
        button_box.Add(self.ref_minus_button, 0, wx.ALIGN_CENTER)
        control_box.Add(button_box, 0, wx.ALIGN_CENTER)
        control_box.AddStretchSpacer()
        #set sizer
        self.SetSizerAndFit(control_box)
        #update
        self.update()
        
    def update(self):
        """
        Read the state of the fft plot settings and update the control panel.
        """
        #update checkboxes
        self.average_check_box.SetValue(self.parent.fftsink.average)
        self.use_persistence_check_box.SetValue(self.parent.fftsink.use_persistence)
        self.peak_hold_check_box.SetValue(self.parent.fftsink.peak_hold)
        #update radio buttons    
        try:
            index = list(DIV_LEVELS).index(self.parent.fftsink.y_per_div)
            self.radio_buttons[index].SetValue(True)
        except: pass
        
    def on_radio_button_change(self, evt):
        selected_radio_button = filter(lambda rb: rb.GetValue(), self.radio_buttons)[0] 
        index = self.radio_buttons.index(selected_radio_button)
        self.parent.fftsink.set_y_per_div(DIV_LEVELS[index])

class fft_window (wx.Panel):
    def __init__ (self, fftsink, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        
        self.fftsink = fftsink
        #init panel and plot 
        wx.Panel.__init__(self, parent, -1)                  
        self.plot = plot.PlotCanvas(self, id, pos, size, style, name)       
        #setup the box with plot and controls
        self.control_panel = control_panel(self)
        main_box = wx.BoxSizer (wx.HORIZONTAL)
        main_box.Add (self.plot, 1, wx.EXPAND)
        main_box.Add (self.control_panel, 0, wx.EXPAND)
        self.SetSizerAndFit(main_box)
        
        self.peak_hold = False
        self.peak_vals = None

        self.use_persistence=False
        self.persist_alpha=0.2

        
        self.plot.SetEnableGrid (True)
        # self.SetEnableZoom (True)
        # self.SetBackgroundColour ('black')
        
        self.build_popup_menu()
        self.set_baseband_freq(self.fftsink.baseband_freq)
                
        EVT_DATA_EVENT (self, self.set_data)
        wx.EVT_CLOSE (self, self.on_close_window)
        self.plot.Bind(wx.EVT_RIGHT_UP, self.on_right_click)
        self.plot.Bind(wx.EVT_MOTION, self.evt_motion)
        
        self.input_watcher = input_watcher(fftsink.msgq, fftsink.fft_size, self)

    def set_scale(self, freq):
        x = max(abs(self.fftsink.sample_rate), abs(self.fftsink.baseband_freq))        
        if x >= 1e9:
            self._scale_factor = 1e-9
            self._units = "GHz"
            self._format = "%3.6f"
        elif x >= 1e6:
            self._scale_factor = 1e-6
            self._units = "MHz"
            self._format = "%3.3f"
        else:
            self._scale_factor = 1e-3
            self._units = "kHz"
            self._format = "%3.3f"

    def set_baseband_freq(self, baseband_freq):
        if self.peak_hold:
            self.peak_vals = None
        self.set_scale(baseband_freq)
        self.fftsink.set_baseband_freq(baseband_freq)
        
    def on_close_window (self, event):
        print "fft_window:on_close_window"
        self.keep_running = False


    def set_data (self, evt):
        dB = evt.data
        L = len (dB)

        if self.peak_hold:
            if self.peak_vals is None:
                self.peak_vals = dB
            else:
                self.peak_vals = numpy.maximum(dB, self.peak_vals)

        if self.fftsink.input_is_real:     # only plot 1/2 the points
            x_vals = ((numpy.arange (L/2) * (self.fftsink.sample_rate 
                       * self._scale_factor / L))
                      + self.fftsink.baseband_freq * self._scale_factor)
            self._points = numpy.zeros((len(x_vals), 2), numpy.float64)
            self._points[:,0] = x_vals
            self._points[:,1] = dB[0:L/2]
            if self.peak_hold:
                self._peak_points = numpy.zeros((len(x_vals), 2), numpy.float64)
                self._peak_points[:,0] = x_vals
                self._peak_points[:,1] = self.peak_vals[0:L/2]
        else:
            # the "negative freqs" are in the second half of the array
            x_vals = ((numpy.arange (-L/2, L/2)
                       * (self.fftsink.sample_rate * self._scale_factor / L))
                      + self.fftsink.baseband_freq * self._scale_factor)
            self._points = numpy.zeros((len(x_vals), 2), numpy.float64)
            self._points[:,0] = x_vals
            self._points[:,1] = numpy.concatenate ((dB[L/2:], dB[0:L/2]))
            if self.peak_hold:
                self._peak_points = numpy.zeros((len(x_vals), 2), numpy.float64)
                self._peak_points[:,0] = x_vals
                self._peak_points[:,1] = numpy.concatenate ((self.peak_vals[L/2:], self.peak_vals[0:L/2]))

        lines = [plot.PolyLine (self._points, colour='BLUE'),]
        if self.peak_hold:
            lines.append(plot.PolyLine (self._peak_points, colour='GREEN'))

        graphics = plot.PlotGraphics (lines,
                                      title=self.fftsink.title,
                                      xLabel = self._units, yLabel = "dB")
        x_range = x_vals[0], x_vals[-1]
        ymax = self.fftsink.ref_level
        ymin = self.fftsink.ref_level - self.fftsink.y_per_div * self.fftsink.y_divs
        y_range = ymin, ymax
        self.plot.Draw (graphics, xAxis=x_range, yAxis=y_range, step=self.fftsink.y_per_div)        

    def set_use_persistence(self, enable):
        self.use_persistence = enable
        self.plot.set_use_persistence( enable)

    def set_persist_alpha(self, persist_alpha):
        self.persist_alpha = persist_alpha
        self.plot.set_persist_alpha(persist_alpha)

    def set_peak_hold(self, enable):
        self.peak_hold = enable
        self.peak_vals = None

    def on_average(self, evt):
        # print "on_average"
        self.fftsink.set_average(evt.IsChecked())
        self.control_panel.update()

    def on_use_persistence(self, evt):
        # print "on_analog"
        self.fftsink.set_use_persistence(evt.IsChecked())
        self.control_panel.update()

    def on_peak_hold(self, evt):
        # print "on_peak_hold"
        self.fftsink.set_peak_hold(evt.IsChecked())
        self.control_panel.update()

    def on_incr_ref_level(self, evt):
        # print "on_incr_ref_level"
        self.fftsink.set_ref_level(self.fftsink.ref_level
                                   + self.fftsink.y_per_div)

    def on_decr_ref_level(self, evt):
        # print "on_decr_ref_level"
        self.fftsink.set_ref_level(self.fftsink.ref_level
                                   - self.fftsink.y_per_div)

    def on_incr_y_per_div(self, evt):
        # print "on_incr_y_per_div"
        self.fftsink.set_y_per_div(next_up(self.fftsink.y_per_div, DIV_LEVELS))
        self.control_panel.update()

    def on_decr_y_per_div(self, evt):
        # print "on_decr_y_per_div"
        self.fftsink.set_y_per_div(next_down(self.fftsink.y_per_div, DIV_LEVELS))
        self.control_panel.update()

    def on_y_per_div(self, evt):
        # print "on_y_per_div"
        Id = evt.GetId()
        if Id == self.id_y_per_div_1:
            self.fftsink.set_y_per_div(1)
        elif Id == self.id_y_per_div_2:
            self.fftsink.set_y_per_div(2)
        elif Id == self.id_y_per_div_5:
            self.fftsink.set_y_per_div(5)
        elif Id == self.id_y_per_div_10:
            self.fftsink.set_y_per_div(10)
        elif Id == self.id_y_per_div_20:
            self.fftsink.set_y_per_div(20)
        self.control_panel.update()

    def on_right_click(self, event):
        menu = self.popup_menu
        for id, pred in self.checkmarks.items():
            item = menu.FindItemById(id)
            item.Check(pred())
        self.plot.PopupMenu(menu, event.GetPosition())

    def evt_motion(self, event):
        if not hasattr(self, "_points"):
            return # Got here before first window data update
            
        # Clip to plotted values
        (ux, uy) = self.plot.GetXY(event)      # Scaled position
        x_vals = numpy.array(self._points[:,0])
        if ux < x_vals[0] or ux > x_vals[-1]:
            tip = self.GetToolTip()
            if tip:
                tip.Enable(False)
            return

        # Get nearest X value (is there a better way)?
        ind = numpy.argmin(numpy.abs(x_vals-ux))
        x_val = x_vals[ind]
        db_val = self._points[ind, 1]
        text = (self._format+" %s dB=%3.3f") % (x_val, self._units, db_val)

        # Display the tooltip
        tip = wx.ToolTip(text)
        tip.Enable(True)
        tip.SetDelay(0)
        self.SetToolTip(tip)
        
    def build_popup_menu(self):
        self.id_incr_ref_level = wx.NewId()
        self.id_decr_ref_level = wx.NewId()
        self.id_incr_y_per_div = wx.NewId()
        self.id_decr_y_per_div = wx.NewId()
        self.id_y_per_div_1 = wx.NewId()
        self.id_y_per_div_2 = wx.NewId()
        self.id_y_per_div_5 = wx.NewId()
        self.id_y_per_div_10 = wx.NewId()
        self.id_y_per_div_20 = wx.NewId()
        self.id_average = wx.NewId()
        self.id_use_persistence = wx.NewId()
        self.id_peak_hold = wx.NewId()
        
        self.plot.Bind(wx.EVT_MENU, self.on_average, id=self.id_average)
        self.plot.Bind(wx.EVT_MENU, self.on_use_persistence, id=self.id_use_persistence)
        self.plot.Bind(wx.EVT_MENU, self.on_peak_hold, id=self.id_peak_hold)
        self.plot.Bind(wx.EVT_MENU, self.on_incr_ref_level, id=self.id_incr_ref_level)
        self.plot.Bind(wx.EVT_MENU, self.on_decr_ref_level, id=self.id_decr_ref_level)
        self.plot.Bind(wx.EVT_MENU, self.on_incr_y_per_div, id=self.id_incr_y_per_div)
        self.plot.Bind(wx.EVT_MENU, self.on_decr_y_per_div, id=self.id_decr_y_per_div)
        self.plot.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_1)
        self.plot.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_2)
        self.plot.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_5)
        self.plot.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_10)
        self.plot.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_20)
        
        # make a menu
        menu = wx.Menu()
        self.popup_menu = menu
        menu.AppendCheckItem(self.id_average, "Average")
        menu.AppendCheckItem(self.id_use_persistence, "Persistence")
        menu.AppendCheckItem(self.id_peak_hold, "Peak Hold")
        menu.Append(self.id_incr_ref_level, "Incr Ref Level")
        menu.Append(self.id_decr_ref_level, "Decr Ref Level")
        # menu.Append(self.id_incr_y_per_div, "Incr dB/div")
        # menu.Append(self.id_decr_y_per_div, "Decr dB/div")
        menu.AppendSeparator()
        # we'd use RadioItems for these, but they're not supported on Mac
        menu.AppendCheckItem(self.id_y_per_div_1, "1 dB/div")
        menu.AppendCheckItem(self.id_y_per_div_2, "2 dB/div")
        menu.AppendCheckItem(self.id_y_per_div_5, "5 dB/div")
        menu.AppendCheckItem(self.id_y_per_div_10, "10 dB/div")
        menu.AppendCheckItem(self.id_y_per_div_20, "20 dB/div")

        self.checkmarks = {
            self.id_average : lambda : self.fftsink.average,
            self.id_use_persistence : lambda : self.fftsink.use_persistence,
            self.id_peak_hold : lambda : self.fftsink.peak_hold,
            self.id_y_per_div_1 : lambda : self.fftsink.y_per_div == 1,
            self.id_y_per_div_2 : lambda : self.fftsink.y_per_div == 2,
            self.id_y_per_div_5 : lambda : self.fftsink.y_per_div == 5,
            self.id_y_per_div_10 : lambda : self.fftsink.y_per_div == 10,
            self.id_y_per_div_20 : lambda : self.fftsink.y_per_div == 20,
            }


def next_up(v, seq):
    """
    Return the first item in seq that is > v.
    """
    for s in seq:
        if s > v:
            return s
    return v

def next_down(v, seq):
    """
    Return the last item in seq that is < v.
    """
    rseq = list(seq[:])
    rseq.reverse()

    for s in rseq:
        if s < v:
            return s
    return v


# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

class test_app_block (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        fft_size = 256

        # build our flow graph
        input_rate = 100*20.48e3

        # Generate a complex sinusoid
        #src1 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 100*2e3, 1)
        src1 = gr.sig_source_c (input_rate, gr.GR_CONST_WAVE, 100*5.75e3, 1)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr1 = gr.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = fft_sink_c (panel, title="Complex Data", fft_size=fft_size,
                            sample_rate=input_rate, baseband_freq=100e3,
                            ref_level=0, y_per_div=20, y_divs=10)
        vbox.Add (sink1.win, 1, wx.EXPAND)

        self.connect(src1, thr1, sink1)

        #src2 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 100*2e3, 1)
        src2 = gr.sig_source_f (input_rate, gr.GR_CONST_WAVE, 100*5.75e3, 1)
        thr2 = gr.throttle(gr.sizeof_float, input_rate)
        sink2 = fft_sink_f (panel, title="Real Data", fft_size=fft_size*2,
                            sample_rate=input_rate, baseband_freq=100e3,
                            ref_level=0, y_per_div=20, y_divs=10)
        vbox.Add (sink2.win, 1, wx.EXPAND)

        self.connect(src2, thr2, sink2)

def main ():
    app = stdgui2.stdapp (test_app_block, "FFT Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
