#!/usr/bin/env python
#
# Copyright 2003,2004,2005,2006,2007,2008 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

from gnuradio import gr, gru, window
from gnuradio.wxgui import stdgui2
import wx
import gnuradio.wxgui.plot as plot
import numpy
import threading
import math    

default_numbersink_size = (640,240)
default_number_rate = gr.prefs().get_long('wxgui', 'number_rate', 15)

class number_sink_base(object):
    def __init__(self, input_is_real=False, unit='',base_value=0, minval=-100.0,maxval=100.0,factor=1.0,decimal_places=10, ref_level=50,
                 sample_rate=1, 
                 number_rate=default_number_rate,
                 average=False, avg_alpha=None, label='', peak_hold=False):

        # initialize common attributes
        self.unit=unit
        self.base_value = base_value
        self.minval=minval
        self.maxval=maxval
        self.factor=factor
        self.y_divs = 8
        self.decimal_places=decimal_places
        self.ref_level = ref_level
        self.sample_rate = sample_rate
        number_size=1
        self.number_size = number_size
        self.number_rate = number_rate
        self.average = average
        if avg_alpha is None:
            self.avg_alpha = 2.0 / number_rate
        else:
            self.avg_alpha = avg_alpha
        self.label = label
        self.peak_hold = peak_hold
        self.show_gauge = True
        self.input_is_real = input_is_real
        self.msgq = gr.msg_queue(2)         # queue that holds a maximum of 2 messages

    def set_decimal_places(self, decimal_places):
        self.decimal_places = decimal_places

    def set_ref_level(self, ref_level):
        self.ref_level = ref_level

    def print_current_value(self, comment):
        print comment,self.win.current_value

    def set_average(self, average):
        self.average = average
        if average:
            self.avg.set_taps(self.avg_alpha)
            self.set_peak_hold(False)
        else:
            self.avg.set_taps(1.0)

    def set_peak_hold(self, enable):
        self.peak_hold = enable
        if enable:
            self.set_average(False)
        self.win.set_peak_hold(enable)

    def set_show_gauge(self, enable):
        self.show_gauge = enable
        self.win.set_show_gauge(enable)

    def set_avg_alpha(self, avg_alpha):
        self.avg_alpha = avg_alpha

    def set_base_value(self, base_value):
        self.base_value = base_value
        

class number_sink_f(gr.hier_block2, number_sink_base):
    def __init__(self, parent, unit='',base_value=0,minval=-100.0,maxval=100.0,factor=1.0,
                 decimal_places=10, ref_level=50, sample_rate=1, 
                 number_rate=default_number_rate, average=False, avg_alpha=None,
                 label='', size=default_numbersink_size, peak_hold=False):

	gr.hier_block2.__init__(self, "number_sink_f",
				gr.io_signature(1, 1, gr.sizeof_float), # Input signature
				gr.io_signature(0, 0, 0))               # Output signature

        number_sink_base.__init__(self, unit=unit, input_is_real=True, base_value=base_value,
                               minval=minval,maxval=maxval,factor=factor,
                               decimal_places=decimal_places, ref_level=ref_level,
                               sample_rate=sample_rate, number_rate=number_rate,
                               average=average, avg_alpha=avg_alpha, label=label,
                               peak_hold=peak_hold)
         
        number_size=1                      
        one_in_n = gr.keep_one_in_n(gr.sizeof_float,
                                    max(1, int(sample_rate/number_rate)))
            
        self.avg = gr.single_pole_iir_filter_ff(1.0, number_size)
        sink = gr.message_sink(gr.sizeof_float , self.msgq, True)
        self.connect(self, self.avg, one_in_n, sink)

        self.win = number_window(self, parent, size=size,label=label)
        self.set_average(self.average)
	self.set_peak_hold(self.peak_hold)
	
class number_sink_c(gr.hier_block2, number_sink_base):
    def __init__(self, parent, unit='',base_value=0,minval=-100.0,maxval=100.0,factor=1.0,
                 decimal_places=10, ref_level=50, sample_rate=1,
                 number_rate=default_number_rate, average=False, avg_alpha=None,
                 label='', size=default_numbersink_size, peak_hold=False):

	gr.hier_block2.__init__(self, "number_sink_c",
				gr.io_signature(1, 1, gr.sizeof_gr_complex), # Input signature
				gr.io_signature(0, 0, 0))                    # Output signature

        number_sink_base.__init__(self, unit=unit, input_is_real=False, base_value=base_value,factor=factor,
                               minval=minval,maxval=maxval,decimal_places=decimal_places, ref_level=ref_level,
                               sample_rate=sample_rate, number_rate=number_rate,
                               average=average, avg_alpha=avg_alpha, label=label,
                               peak_hold=peak_hold)

        number_size=1                      
        one_in_n = gr.keep_one_in_n(gr.sizeof_gr_complex,
                                    max(1, int(sample_rate/number_rate)))
            
        self.avg = gr.single_pole_iir_filter_cc(1.0, number_size)
        sink = gr.message_sink(gr.sizeof_gr_complex , self.msgq, True)
        self.connect(self, self.avg, one_in_n, sink)

        self.win = number_window(self, parent, size=size,label=label)
        self.set_average(self.average)
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


class input_watcher (threading.Thread):
    def __init__ (self, msgq, number_size, event_receiver, **kwds):
        threading.Thread.__init__ (self, **kwds)
        self.setDaemon (1)
        self.msgq = msgq
        self.number_size = number_size
        self.event_receiver = event_receiver
        self.keep_running = True
        self.start ()

    def run (self):
        while (self.keep_running):
            msg = self.msgq.delete_head()  # blocking read of message queue
            itemsize = int(msg.arg1())
            nitems = int(msg.arg2())

            s = msg.to_string()            # get the body of the msg as a string

            # There may be more than one number in the message.
            # If so, we take only the last one
            if nitems > 1:
                start = itemsize * (nitems - 1)
                s = s[start:start+itemsize]

            complex_data = numpy.fromstring (s, numpy.float32)
            de = DataEvent (complex_data)
            wx.PostEvent (self.event_receiver, de)
            del de
    
#========================================================================================
class static_text_window (wx.StaticText): #plot.PlotCanvas):
    def __init__ (self, parent, numbersink,id = -1,label="number",
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        wx.StaticText.__init__(self, parent, id, label, pos, size, style, name)
        self.parent=parent
        self.label=label
        self.numbersink = numbersink
        self.peak_hold = False
        self.peak_vals = None
        self.build_popup_menu()
        self.Bind(wx.EVT_RIGHT_UP, self.on_right_click)

    def on_close_window (self, event):
        print "number_window:on_close_window"
        self.keep_running = False

    def set_peak_hold(self, enable):
        self.peak_hold = enable
        self.peak_vals = None

    def update_y_range (self):
        ymax = self.numbersink.ref_level
        ymin = self.numbersink.ref_level - self.numbersink.decimal_places * self.numbersink.y_divs
        self.y_range = self._axisInterval ('min', ymin, ymax)

    def on_average(self, evt):
        # print "on_average"
        self.numbersink.set_average(evt.IsChecked())

    def on_peak_hold(self, evt):
        # print "on_peak_hold"
        self.numbersink.set_peak_hold(evt.IsChecked())

    def on_show_gauge(self, evt):
        # print "on_show_gauge"
        self.numbersink.set_show_gauge(evt.IsChecked())
        print evt.IsChecked()

    def on_incr_ref_level(self, evt):
        # print "on_incr_ref_level"
        self.numbersink.set_ref_level(self.numbersink.ref_level
                                   + self.numbersink.decimal_places)

    def on_decr_ref_level(self, evt):
        # print "on_decr_ref_level"
        self.numbersink.set_ref_level(self.numbersink.ref_level
                                   - self.numbersink.decimal_places)

    def on_incr_decimal_places(self, evt):
        # print "on_incr_decimal_places"
        self.numbersink.set_decimal_places(self.numbersink.decimal_places+1)

    def on_decr_decimal_places(self, evt):
        # print "on_decr_decimal_places"
        self.numbersink.set_decimal_places(max(self.numbersink.decimal_places-1,0)) 

    def on_decimal_places(self, evt):
        # print "on_decimal_places"
        Id = evt.GetId()
        if Id == self.id_decimal_places_0:
            self.numbersink.set_decimal_places(0)
        elif Id == self.id_decimal_places_1:
            self.numbersink.set_decimal_places(1)
        elif Id == self.id_decimal_places_2:
            self.numbersink.set_decimal_places(2)
        elif Id == self.id_decimal_places_3:
            self.numbersink.set_decimal_places(3)
        elif Id == self.id_decimal_places_6:
            self.numbersink.set_decimal_places(6)
        elif Id == self.id_decimal_places_9:
            self.numbersink.set_decimal_places(9)
        
    def on_right_click(self, event):
        menu = self.popup_menu
        for id, pred in self.checkmarks.items():
            item = menu.FindItemById(id)
            item.Check(pred())
        self.PopupMenu(menu, event.GetPosition())

    def build_popup_menu(self):
        self.id_show_gauge = wx.NewId()
        self.id_incr_ref_level = wx.NewId()
        self.id_decr_ref_level = wx.NewId()
        self.id_incr_decimal_places = wx.NewId()
        self.id_decr_decimal_places = wx.NewId()
        self.id_decimal_places_0 = wx.NewId()
        self.id_decimal_places_1 = wx.NewId()
        self.id_decimal_places_2 = wx.NewId()
        self.id_decimal_places_3 = wx.NewId()
        self.id_decimal_places_6 = wx.NewId()
        self.id_decimal_places_9 = wx.NewId()
        self.id_average = wx.NewId()
        self.id_peak_hold = wx.NewId()

        self.Bind(wx.EVT_MENU, self.on_average, id=self.id_average)
        self.Bind(wx.EVT_MENU, self.on_peak_hold, id=self.id_peak_hold)
        #self.Bind(wx.EVT_MENU, self.on_hide_gauge, id=self.id_hide_gauge)
        self.Bind(wx.EVT_MENU, self.on_show_gauge, id=self.id_show_gauge)
        self.Bind(wx.EVT_MENU, self.on_incr_ref_level, id=self.id_incr_ref_level)
        self.Bind(wx.EVT_MENU, self.on_decr_ref_level, id=self.id_decr_ref_level)
        self.Bind(wx.EVT_MENU, self.on_incr_decimal_places, id=self.id_incr_decimal_places)
        self.Bind(wx.EVT_MENU, self.on_decr_decimal_places, id=self.id_decr_decimal_places)
        self.Bind(wx.EVT_MENU, self.on_decimal_places, id=self.id_decimal_places_0)
        self.Bind(wx.EVT_MENU, self.on_decimal_places, id=self.id_decimal_places_1)
        self.Bind(wx.EVT_MENU, self.on_decimal_places, id=self.id_decimal_places_2)
        self.Bind(wx.EVT_MENU, self.on_decimal_places, id=self.id_decimal_places_3)
        self.Bind(wx.EVT_MENU, self.on_decimal_places, id=self.id_decimal_places_6)
        self.Bind(wx.EVT_MENU, self.on_decimal_places, id=self.id_decimal_places_9)

        # make a menu
        menu = wx.Menu()
        self.popup_menu = menu
        menu.AppendCheckItem(self.id_average, "Average")
        menu.AppendCheckItem(self.id_peak_hold, "Peak Hold")
        menu.AppendCheckItem(self.id_show_gauge, "Show gauge")
        menu.Append(self.id_incr_ref_level, "Incr Ref Level")
        menu.Append(self.id_decr_ref_level, "Decr Ref Level")
        menu.Append(self.id_incr_decimal_places, "Incr decimal places")
        menu.Append(self.id_decr_decimal_places, "Decr decimal places")
        menu.AppendSeparator()
        # we'd use RadioItems for these, but they're not supported on Mac
        menu.AppendCheckItem(self.id_decimal_places_0, "0 decimal places")
        menu.AppendCheckItem(self.id_decimal_places_1, "1 decimal places")
        menu.AppendCheckItem(self.id_decimal_places_2, "2 decimal places")
        menu.AppendCheckItem(self.id_decimal_places_3, "3 decimal places")
        menu.AppendCheckItem(self.id_decimal_places_6, "6 decimal places")
        menu.AppendCheckItem(self.id_decimal_places_9, "9 decimal places")

        self.checkmarks = {
            self.id_average : lambda : self.numbersink.average,
            self.id_peak_hold : lambda : self.numbersink.peak_hold,
            self.id_show_gauge : lambda : self.numbersink.show_gauge,
            self.id_decimal_places_0 : lambda : self.numbersink.decimal_places == 0,
            self.id_decimal_places_1 : lambda : self.numbersink.decimal_places == 1,
            self.id_decimal_places_2 : lambda : self.numbersink.decimal_places == 2,
            self.id_decimal_places_3 : lambda : self.numbersink.decimal_places == 3,
            self.id_decimal_places_6 : lambda : self.numbersink.decimal_places == 6,
            self.id_decimal_places_9 : lambda : self.numbersink.decimal_places == 9,
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


#========================================================================================
class number_window (plot.PlotCanvas):
    def __init__ (self, numbersink, parent, id = -1,label="number",
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        plot.PlotCanvas.__init__ (self, parent, id, pos, size, style, name)
        self.static_text=static_text_window( self, numbersink,id, label, pos, (size[0]/2,size[1]/2), style, name)
        gauge_style = wx.GA_HORIZONTAL
        vbox=wx.BoxSizer(wx.VERTICAL)
        vbox.Add (self.static_text, 0, wx.EXPAND)
        self.current_value=None
        if numbersink.input_is_real:
          self.gauge=wx.Gauge( self, id, range=1000, pos=(pos[0],pos[1]+size[1]/2),size=(size[0]/2,size[1]/2), style=gauge_style,  name = "gauge")
          vbox.Add (self.gauge, 1, wx.EXPAND)
        else:
          self.gauge=wx.Gauge( self, id, range=1000, pos=(pos[0],pos[1]+size[1]/3),size=(size[0]/2,size[1]/3), style=gauge_style,  name = "gauge")
          self.gauge_imag=wx.Gauge( self, id, range=1000, pos=(pos[0],pos[1]+size[1]*2/3),size=(size[0]/2,size[1]/3), style=gauge_style,  name = "gauge_imag")
          vbox.Add (self.gauge, 1, wx.EXPAND)
          vbox.Add (self.gauge_imag, 1, wx.EXPAND)
        self.sizer = vbox
        self.SetSizer (self.sizer)
        self.SetAutoLayout (True)
        self.sizer.Fit (self)

        self.label=label
        self.numbersink = numbersink
        self.peak_hold = False
        self.peak_vals = None
        
        EVT_DATA_EVENT (self, self.set_data)
        wx.EVT_CLOSE (self, self.on_close_window)
        self.input_watcher = input_watcher(numbersink.msgq, numbersink.number_size, self)

    def on_close_window (self, event):
        # print "number_window:on_close_window"
        self.keep_running = False

    def set_show_gauge(self, enable):
        self.show_gauge = enable
        if enable:
          self.gauge.Show()
          if not self.numbersink.input_is_real:
            self.gauge_imag.Show()
          #print 'show'
        else:
          self.gauge.Hide()
          if not self.numbersink.input_is_real:
            self.gauge_imag.Hide()
          #print 'hide'

    def set_data (self, evt):
        numbers = evt.data
        L = len (numbers)

        if self.peak_hold:
            if self.peak_vals is None:
                self.peak_vals = numbers
            else:
                self.peak_vals = numpy.maximum(numbers, self.peak_vals)
                numbers = self.peak_vals

        if self.numbersink.input_is_real:
            real_value=numbers[0]*self.numbersink.factor + self.numbersink.base_value
            imag_value=0.0
            self.current_value=real_value
        else:
            real_value=numbers[0]*self.numbersink.factor + self.numbersink.base_value
            imag_value=numbers[1]*self.numbersink.factor + self.numbersink.base_value
            self.current_value=complex(real_value,imag_value)
        x = max(real_value, imag_value)
        if x >= 1e9:
            sf = 1e-9
            unit_prefix = "G"
        elif x >= 1e6:
            sf = 1e-6
            unit_prefix = "M"
        elif x>= 1e3:
            sf = 1e-3
            unit_prefix = "k"
        else :
            sf = 1
            unit_prefix = ""
        if self.numbersink.input_is_real:
          showtext = "%s: %.*f %s%s" % (self.label, self.numbersink.decimal_places,real_value*sf,unit_prefix,self.numbersink.unit)
        else:
          showtext = "%s: %.*f,%.*f %s%s" % (self.label, self.numbersink.decimal_places,real_value*sf,
                                                       self.numbersink.decimal_places,imag_value*sf,unit_prefix,self.numbersink.unit)
        self.static_text.SetLabel(showtext)
        self.gauge.SetValue(int(float((real_value-self.numbersink.base_value)*1000.0/(self.numbersink.maxval-self.numbersink.minval)))+500)
        if not self.numbersink.input_is_real:
          self.gauge.SetValue(int(float((imag_value-self.numbersink.base_value)*1000.0/(self.numbersink.maxval-self.numbersink.minval)))+500)

    def set_peak_hold(self, enable):
        self.peak_hold = enable
        self.peak_vals = None

    def update_y_range (self):
        ymax = self.numbersink.ref_level
        ymin = self.numbersink.ref_level - self.numbersink.decimal_places * self.numbersink.y_divs
        self.y_range = self._axisInterval ('min', ymin, ymax)

    def on_average(self, evt):
        # print "on_average"
        self.numbersink.set_average(evt.IsChecked())

    def on_peak_hold(self, evt):
        # print "on_peak_hold"
        self.numbersink.set_peak_hold(evt.IsChecked())


# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

class test_app_flow_graph (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        # build our flow graph
        input_rate = 20.48e3

        # Generate a real and complex sinusoids
        src1 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 2e3, 1)
        src2 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 2e3, 1)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr1 = gr.throttle(gr.sizeof_float, input_rate)
        thr2 = gr.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = number_sink_f (panel, unit='Hz',label="Real Data", avg_alpha=0.001,
                            sample_rate=input_rate, base_value=100e3,
                            ref_level=0, decimal_places=3)
        vbox.Add (sink1.win, 1, wx.EXPAND)
        sink2 = number_sink_c (panel, unit='V',label="Complex Data", avg_alpha=0.001,
                            sample_rate=input_rate, base_value=0,
                            ref_level=0, decimal_places=3)
        vbox.Add (sink2.win, 1, wx.EXPAND)

        self.connect (src1, thr1, sink1)
        self.connect (src2, thr2, sink2)

def main ():
    app = stdgui2.stdapp (test_app_flow_graph, "Number Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
