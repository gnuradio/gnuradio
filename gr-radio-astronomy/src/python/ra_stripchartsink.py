#!/usr/bin/env python
#
# Copyright 2003,2004,2005 Free Software Foundation, Inc.
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
from gnuradio.wxgui import stdgui
import wx
import gnuradio.wxgui.plot as plot
import Numeric
import threading
import math    
import ephem
import time

default_stripchartsink_size = (640,140)
global_yvalues = []

class stripchart_sink_base(object):
    def __init__(self, input_is_real=False, y_per_div=10, ref_level=50,
                 sample_rate=1, stripsize=4,
                 title='',xlabel="X", ylabel="Y", divbase=0.025,
                 parallel=False, scaling=1.0, autoscale=False):

        # initialize common attributes
        self.y_divs = 8
        self.y_per_div=y_per_div
        self.ref_level = ref_level
        self.autoscale = autoscale
        self.sample_rate = sample_rate
        self.parallel = parallel
        self.title = title
        self.xlabel = xlabel
        self.ylabel = ylabel
        self.divbase = divbase
        self.scaling = scaling
        self.input_is_real = input_is_real
        self.msgq = gr.msg_queue(2)         # queue that holds a maximum of 2 messages
        self.vector=Numeric.zeros(stripsize,Numeric.Float64)
        self.wcnt = 0
        self.timecnt = 0
        self.stripsize=stripsize

    def set_y_per_div(self, y_per_div):
        self.y_per_div = y_per_div

    def set_ref_level(self, ref_level):
        self.ref_level = ref_level

    def set_autoscale(self, auto):
        self.autoscale = auto

class stripchart_sink_f(gr.hier_block, stripchart_sink_base):
    def __init__(self, fg, parent,
                 y_per_div=10, ref_level=50, sample_rate=1,
                 title='', stripsize=4,
                 size=default_stripchartsink_size,xlabel="X", 
                 ylabel="Y", divbase=0.025,
                 parallel=False, scaling=1.0, autoscale=False):

        stripchart_sink_base.__init__(self, input_is_real=True,
                               y_per_div=y_per_div, ref_level=ref_level,
                               sample_rate=sample_rate,
                               stripsize=stripsize,
                               xlabel=xlabel, ylabel=ylabel, 
                               divbase=divbase, title=title,
                               parallel=parallel, 
                               scaling=scaling, autoscale=autoscale)
                               
        if (parallel == True):
            one = gr.keep_one_in_n (gr.sizeof_float*stripsize, 1)
            sink = gr.message_sink(gr.sizeof_float*stripsize, self.msgq, True)
        else:
            one = gr.keep_one_in_n (gr.sizeof_float, 1)
            sink = gr.message_sink(gr.sizeof_float, self.msgq, True)
        fg.connect (one, sink)

        gr.hier_block.__init__(self, fg, one, sink)

        self.win = stripchart_window(self, parent, size=size)



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
    def __init__ (self, msgq, evsize, event_receiver, **kwds):
        threading.Thread.__init__ (self, **kwds)
        self.setDaemon (1)
        self.msgq = msgq
        self.evsize = evsize
        self.event_receiver = event_receiver
        self.keep_running = True
        self.start ()

    def run (self):
        while (self.keep_running):
            msg = self.msgq.delete_head()  # blocking read of message queue
            itemsize = int(msg.arg1())
            nitems = int(msg.arg2())

            s = msg.to_string()            # get the body of the msg as a string

            # There may be more than one frame in the message.
            # If so, we take only the last one
            if nitems > 1:
                start = itemsize * (nitems - 1)
                s = s[start:start+itemsize]

            complex_data = Numeric.fromstring (s, Numeric.Float32)
            de = DataEvent (complex_data)
            wx.PostEvent (self.event_receiver, de)
            del de

class stripchart_window(plot.PlotCanvas):
    def __init__ (self, stripchartsink, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        plot.PlotCanvas.__init__ (self, parent, id, pos, size, style, name)

        self.y_range = None
        self.stripchartsink = stripchartsink

        self.SetEnableGrid (True)
        # self.SetEnableZoom (True)
        # self.SetBackgroundColour ('black')
        
        self.build_popup_menu()
        
        EVT_DATA_EVENT (self, self.set_data)

        wx.EVT_CLOSE (self, self.on_close_window)
        self.Bind(wx.EVT_RIGHT_UP, self.on_right_click)

        self.input_watcher = input_watcher(stripchartsink.msgq, 1, self)


    def on_close_window (self, event):
        print "stripchart_window:on_close_window"
        self.keep_running = False


    def set_data (self, evt):
        indata = evt.data
        L = len (indata)

        calc_min = min(indata)
        calc_max = max(indata)
        d = calc_max - calc_min
        d = d * 0.1
        if self.stripchartsink.autoscale == True and self.stripchartsink.parallel == True:
            self.y_range = self._axisInterval ('min', calc_min-d, calc_max+d)

        N = self.stripchartsink.stripsize
        if self.stripchartsink.parallel != True:
    	    for i in range(1,N):
                pooey = N-i
                self.stripchartsink.vector[pooey] = self.stripchartsink.vector[pooey-1]
    
            self.stripchartsink.vector[0] = indata

        else:
            self.stripchartsink.vector = indata

        if self.stripchartsink.parallel == True:
            avg = 0
            for i in range(0,self.stripchartsink.stripsize):
                if self.stripchartsink.vector[i] > 0:
                    avg += self.stripchartsink.vector[i]
                if self.stripchartsink.vector[i] < calc_min:
                    calc_min = self.stripchartsink.vector[i]
                if self.stripchartsink.vector[i] > calc_max:
                    calc_max = self.stripchartsink.vector[i]

            avg /= self.stripchartsink.stripsize
            markers = []
            placedmarkers = 0
            for i in range(0,self.stripchartsink.stripsize):
                if (self.stripchartsink.vector[i] > 0 and
                    self.stripchartsink.vector[i] > (avg*5)):
                       markers.append((i*self.stripchartsink.scaling,
                       self.stripchartsink.vector[i]))
                       placedmarkers += 1
            
        points = Numeric.zeros((N,2), Numeric.Float64)
        for i in range(0,N):
            if self.stripchartsink.scaling == 1.0:
                points[i,0] = i
            else:
                points[i,0] = i * self.stripchartsink.scaling
            points[i,1] = self.stripchartsink.vector[i]

        if self.stripchartsink.parallel == True and placedmarkers > 1:
            for i in range(0,N):
                self.stripchartsink.vector[i] = 0

            marks = plot.PolyMarker(markers, colour='BLACK', marker='triangle_down')

        lines = plot.PolyLine (points, colour='RED')

        # Temporary--I'm find the markers distracting
        placedmarkers = 0
        xlab = self.stripchartsink.xlabel
        ylab = self.stripchartsink.ylabel
        if (self.stripchartsink.parallel == False) or (placedmarkers <= 1):
            graphics = plot.PlotGraphics ([lines],
                                      title=self.stripchartsink.title,
                                      xLabel = xlab, yLabel = ylab)

        else:
            graphics = plot.PlotGraphics ([lines,marks],
                                      title=self.stripchartsink.title,
                                      xLabel = xlab, yLabel = ylab)

        self.Draw (graphics, xAxis=None, yAxis=self.y_range)

        if self.stripchartsink.autoscale == False or self.stripchartsink.parallel == False:
            self.update_y_range ()


    def update_y_range (self):
        ymax = self.stripchartsink.ref_level
        ymin = self.stripchartsink.ref_level - self.stripchartsink.y_per_div * self.stripchartsink.y_divs
        self.y_range = self._axisInterval ('min', ymin, ymax)

    def on_incr_ref_level(self, evt):
        # print "on_incr_ref_level"
        self.stripchartsink.set_ref_level(self.stripchartsink.ref_level
                                   + self.stripchartsink.y_per_div)

    def on_decr_ref_level(self, evt):
        # print "on_decr_ref_level"
        self.stripchartsink.set_ref_level(self.stripchartsink.ref_level
                                   - self.stripchartsink.y_per_div)

    def on_autoscale(self, evt):
        self.stripchartsink.set_autoscale(evt.IsChecked())

    def on_incr_y_per_div(self, evt):
        divbase = self.stripchartsink.divbase
        x1 = 1 * divbase
        x2 = 2 * divbase
        x4 = 4 * divbase
        x10 = 10 * divbase
        x20 = 20 * divbase
        # print "on_incr_y_per_div"
        self.stripchartsink.set_y_per_div(next_up(self.stripchartsink.y_per_div, (x1,x2,x4,x10,x20)))

    def on_decr_y_per_div(self, evt):
        # print "on_decr_y_per_div"
        divbase = self.stripchartsink.divbase
        x1 = 1 * divbase
        x2 = 2 * divbase
        x4 = 4 * divbase
        x10 = 10 * divbase
        x20 = 20 * divbase
        self.stripchartsink.set_y_per_div(next_down(self.stripchartsink.y_per_div, (x1,x2,x4,x10,x20)))

    def on_y_per_div(self, evt):
        # print "on_y_per_div"
        divbase=self.stripchartsink.divbase
        Id = evt.GetId()
        if Id == self.id_y_per_div_1:
            self.stripchartsink.set_y_per_div(1*divbase)
        elif Id == self.id_y_per_div_2:
            self.stripchartsink.set_y_per_div(2*divbase)
        elif Id == self.id_y_per_div_5:
            self.stripchartsink.set_y_per_div(4*divbase)
        elif Id == self.id_y_per_div_10:
            self.stripchartsink.set_y_per_div(10*divbase)
        elif Id == self.id_y_per_div_20:
            self.stripchartsink.set_y_per_div(20*divbase)

        
    def on_right_click(self, event):
        menu = self.popup_menu
        for id, pred in self.checkmarks.items():
            item = menu.FindItemById(id)
            item.Check(pred())
        self.PopupMenu(menu, event.GetPosition())


    def build_popup_menu(self):
        divbase=self.stripchartsink.divbase
        self.id_incr_ref_level = wx.NewId()
        self.id_decr_ref_level = wx.NewId()
        self.id_autoscale = wx.NewId()
        self.id_incr_y_per_div = wx.NewId()
        self.id_decr_y_per_div = wx.NewId()
        self.id_y_per_div_1 = wx.NewId()
        self.id_y_per_div_2 = wx.NewId()
        self.id_y_per_div_5 = wx.NewId()
        self.id_y_per_div_10 = wx.NewId()
        self.id_y_per_div_20 = wx.NewId()

        self.Bind(wx.EVT_MENU, self.on_incr_ref_level, id=self.id_incr_ref_level)
        self.Bind(wx.EVT_MENU, self.on_decr_ref_level, id=self.id_decr_ref_level)
        self.Bind(wx.EVT_MENU, self.on_autoscale, id=self.id_autoscale)
        self.Bind(wx.EVT_MENU, self.on_incr_y_per_div, id=self.id_incr_y_per_div)
        self.Bind(wx.EVT_MENU, self.on_decr_y_per_div, id=self.id_decr_y_per_div)
        self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_1)
        self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_2)
        self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_5)
        self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_10)
        self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_20)


        # make a menu
        menu = wx.Menu()
        self.popup_menu = menu
        menu.Append(self.id_incr_ref_level, "Incr Ref Level")
        menu.Append(self.id_decr_ref_level, "Decr Ref Level")
        menu.AppendSeparator()
        menu.AppendCheckItem(self.id_autoscale, "Auto Scale")
        # we'd use RadioItems for these, but they're not supported on Mac
        v = 1.0*divbase
        s = "%.3f" % v
        menu.AppendCheckItem(self.id_y_per_div_1, s)
        v = 2.0*divbase
        s = "%.3f" % v
        menu.AppendCheckItem(self.id_y_per_div_2, s)
        v = 4.0*divbase
        s = "%.3f" % v
        menu.AppendCheckItem(self.id_y_per_div_5, s)
        v = 10*divbase
        s = "%.3f" % v
        menu.AppendCheckItem(self.id_y_per_div_10, s)
        v = 20*divbase
        s = "%.3f" % v
        menu.AppendCheckItem(self.id_y_per_div_20, s)

        self.checkmarks = {
            self.id_autoscale : lambda : self.stripchartsink.autoscale,
            self.id_y_per_div_1 : lambda : self.stripchartsink.y_per_div == 1*divbase,
            self.id_y_per_div_2 : lambda : self.stripchartsink.y_per_div == 2*divbase,
            self.id_y_per_div_5 : lambda : self.stripchartsink.y_per_div == 4*divbase,
            self.id_y_per_div_10 : lambda : self.stripchartsink.y_per_div == 10*divbase,
            self.id_y_per_div_20 : lambda : self.stripchartsink.y_per_div == 20*divbase,
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
