#!/usr/bin/env python
#
# Copyright 2003,2004,2005,2006,2007 Free Software Foundation, Inc.
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
from gnuradio.wxgui import stdgui
import wx
import gnuradio.wxgui.plot as plot
import numpy
import threading
import math    

default_fftsink_size = (640,240)
default_fft_rate = gr.prefs().get_long('wxgui', 'fft_rate', 15)

class fft_sink_base(object):
    def __init__(self, input_is_real=False, baseband_freq=0, y_per_div=10, ref_level=50,
                 sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate,
                 average=False, avg_alpha=None, title='', peak_hold=False):

        # initialize common attributes
        self.baseband_freq = baseband_freq
        self.y_divs = 8
        self.y_per_div=y_per_div
        self.ref_level = ref_level
        self.sample_rate = sample_rate
        self.fft_size = fft_size
        self.fft_rate = fft_rate
        self.average = average
        if avg_alpha is None:
            self.avg_alpha = 2.0 / fft_rate
        else:
            self.avg_alpha = avg_alpha
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
            self.set_peak_hold(False)
        else:
            self.avg.set_taps(1.0)

    def set_peak_hold(self, enable):
        self.peak_hold = enable
        if enable:
            self.set_average(False)
        self.win.set_peak_hold(enable)

    def set_avg_alpha(self, avg_alpha):
        self.avg_alpha = avg_alpha

    def set_baseband_freq(self, baseband_freq):
        self.baseband_freq = baseband_freq

    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
        self._set_n()

    def _set_n(self):
        self.one_in_n.set_n(max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        

class fft_sink_f(gr.hier_block, fft_sink_base):
    def __init__(self, fg, parent, baseband_freq=0,
                 y_per_div=10, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, peak_hold=False):

        fft_sink_base.__init__(self, input_is_real=True, baseband_freq=baseband_freq,
                               y_per_div=y_per_div, ref_level=ref_level,
                               sample_rate=sample_rate, fft_size=fft_size,
                               fft_rate=fft_rate,
                               average=average, avg_alpha=avg_alpha, title=title,
                               peak_hold=peak_hold)
                               
        s2p = gr.stream_to_vector(gr.sizeof_float, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_float * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))

        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vfc(self.fft_size, True, mywindow)
        power = 0
        for tap in mywindow:
            power += tap*tap
            
        c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)

        # FIXME  We need to add 3dB to all bins but the DC bin
        log = gr.nlog10_ff(20, self.fft_size,
                           -20*math.log10(self.fft_size)-10*math.log10(power/self.fft_size))
        sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)

        fg.connect (s2p, self.one_in_n, fft, c2mag, self.avg, log, sink)
        gr.hier_block.__init__(self, fg, s2p, sink)

        self.win = fft_window(self, parent, size=size)
        self.set_average(self.average)


class fft_sink_c(gr.hier_block, fft_sink_base):
    def __init__(self, fg, parent, baseband_freq=0,
                 y_per_div=10, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, peak_hold=False):

        fft_sink_base.__init__(self, input_is_real=False, baseband_freq=baseband_freq,
                               y_per_div=y_per_div, ref_level=ref_level,
                               sample_rate=sample_rate, fft_size=fft_size,
                               fft_rate=fft_rate,
                               average=average, avg_alpha=avg_alpha, title=title,
                               peak_hold=peak_hold)

        s2p = gr.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_gr_complex * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        mywindow = window.blackmanharris(self.fft_size)
        power = 0
        for tap in mywindow:
            power += tap*tap
            
        fft = gr.fft_vcc(self.fft_size, True, mywindow)
        c2mag = gr.complex_to_mag(fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, fft_size)
        log = gr.nlog10_ff(20, self.fft_size,
                           -20*math.log10(self.fft_size)-10*math.log10(power/self.fft_size))
        sink = gr.message_sink(gr.sizeof_float * fft_size, self.msgq, True)

        fg.connect(s2p, self.one_in_n, fft, c2mag, self.avg, log, sink)
        gr.hier_block.__init__(self, fg, s2p, sink)

        self.win = fft_window(self, parent, size=size)
        self.set_average(self.average)


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
    def __init__ (self, msgq, fft_size, event_receiver, **kwds):
        threading.Thread.__init__ (self, **kwds)
        self.setDaemon (1)
        self.msgq = msgq
        self.fft_size = fft_size
        self.event_receiver = event_receiver
        self.keep_running = True
        self.start ()

    def run (self):
        while (self.keep_running):
            msg = self.msgq.delete_head()  # blocking read of message queue
            itemsize = int(msg.arg1())
            nitems = int(msg.arg2())

            s = msg.to_string()            # get the body of the msg as a string

            # There may be more than one FFT frame in the message.
            # If so, we take only the last one
            if nitems > 1:
                start = itemsize * (nitems - 1)
                s = s[start:start+itemsize]

            complex_data = numpy.fromstring (s, numpy.float32)
            de = DataEvent (complex_data)
            wx.PostEvent (self.event_receiver, de)
            del de
    

class fft_window (plot.PlotCanvas):
    def __init__ (self, fftsink, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        plot.PlotCanvas.__init__ (self, parent, id, pos, size, style, name)

        self.y_range = None
        self.fftsink = fftsink
        self.peak_hold = False
        self.peak_vals = None

        self.SetEnableGrid (True)
        # self.SetEnableZoom (True)
        # self.SetBackgroundColour ('black')
        
        self.build_popup_menu()
        
        EVT_DATA_EVENT (self, self.set_data)
        wx.EVT_CLOSE (self, self.on_close_window)
        self.Bind(wx.EVT_RIGHT_UP, self.on_right_click)

        self.input_watcher = input_watcher(fftsink.msgq, fftsink.fft_size, self)


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
                dB = self.peak_vals

        x = max(abs(self.fftsink.sample_rate), abs(self.fftsink.baseband_freq))
        if x >= 1e9:
            sf = 1e-9
            units = "GHz"
        elif x >= 1e6:
            sf = 1e-6
            units = "MHz"
        else:
            sf = 1e-3
            units = "kHz"

        if self.fftsink.input_is_real:     # only plot 1/2 the points
            x_vals = ((numpy.arange (L/2)
                       * (self.fftsink.sample_rate * sf / L))
                      + self.fftsink.baseband_freq * sf)
            points = numpy.zeros((len(x_vals), 2), numpy.float64)
            points[:,0] = x_vals
            points[:,1] = dB[0:L/2]
        else:
            # the "negative freqs" are in the second half of the array
            x_vals = ((numpy.arange (-L/2, L/2)
                       * (self.fftsink.sample_rate * sf / L))
                      + self.fftsink.baseband_freq * sf)
            points = numpy.zeros((len(x_vals), 2), numpy.float64)
            points[:,0] = x_vals
            points[:,1] = numpy.concatenate ((dB[L/2:], dB[0:L/2]))


        lines = plot.PolyLine (points, colour='BLUE')

        graphics = plot.PlotGraphics ([lines],
                                      title=self.fftsink.title,
                                      xLabel = units, yLabel = "dB")

        self.Draw (graphics, xAxis=None, yAxis=self.y_range)
        self.update_y_range ()

    def set_peak_hold(self, enable):
        self.peak_hold = enable
        self.peak_vals = None

    def update_y_range (self):
        ymax = self.fftsink.ref_level
        ymin = self.fftsink.ref_level - self.fftsink.y_per_div * self.fftsink.y_divs
        self.y_range = self._axisInterval ('min', ymin, ymax)

    def on_average(self, evt):
        # print "on_average"
        self.fftsink.set_average(evt.IsChecked())

    def on_peak_hold(self, evt):
        # print "on_peak_hold"
        self.fftsink.set_peak_hold(evt.IsChecked())

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
        self.fftsink.set_y_per_div(next_up(self.fftsink.y_per_div, (1,2,5,10,20)))

    def on_decr_y_per_div(self, evt):
        # print "on_decr_y_per_div"
        self.fftsink.set_y_per_div(next_down(self.fftsink.y_per_div, (1,2,5,10,20)))

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

        
    def on_right_click(self, event):
        menu = self.popup_menu
        for id, pred in self.checkmarks.items():
            item = menu.FindItemById(id)
            item.Check(pred())
        self.PopupMenu(menu, event.GetPosition())


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
        self.id_peak_hold = wx.NewId()

        self.Bind(wx.EVT_MENU, self.on_average, id=self.id_average)
        self.Bind(wx.EVT_MENU, self.on_peak_hold, id=self.id_peak_hold)
        self.Bind(wx.EVT_MENU, self.on_incr_ref_level, id=self.id_incr_ref_level)
        self.Bind(wx.EVT_MENU, self.on_decr_ref_level, id=self.id_decr_ref_level)
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
        menu.AppendCheckItem(self.id_average, "Average")
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
#          	      Deprecated interfaces
# ----------------------------------------------------------------

# returns (block, win).
#   block requires a single input stream of float
#   win is a subclass of wxWindow

def make_fft_sink_f(fg, parent, title, fft_size, input_rate, ymin = 0, ymax=50):
    
    block = fft_sink_f(fg, parent, title=title, fft_size=fft_size, sample_rate=input_rate,
                       y_per_div=(ymax - ymin)/8, ref_level=ymax)
    return (block, block.win)

# returns (block, win).
#   block requires a single input stream of gr_complex
#   win is a subclass of wxWindow

def make_fft_sink_c(fg, parent, title, fft_size, input_rate, ymin=0, ymax=50):
    block = fft_sink_c(fg, parent, title=title, fft_size=fft_size, sample_rate=input_rate,
                       y_per_div=(ymax - ymin)/8, ref_level=ymax)
    return (block, block.win)


# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

class test_app_flow_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

        fft_size = 256

        # build our flow graph
        input_rate = 20.48e3

        # Generate a complex sinusoid
        #src1 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 2e3, 1)
        src1 = gr.sig_source_c (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr1 = gr.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = fft_sink_c (self, panel, title="Complex Data", fft_size=fft_size,
                            sample_rate=input_rate, baseband_freq=100e3,
                            ref_level=0, y_per_div=20)
        vbox.Add (sink1.win, 1, wx.EXPAND)
        self.connect (src1, thr1, sink1)

        #src2 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 2e3, 1)
        src2 = gr.sig_source_f (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1)
        thr2 = gr.throttle(gr.sizeof_float, input_rate)
        sink2 = fft_sink_f (self, panel, title="Real Data", fft_size=fft_size*2,
                            sample_rate=input_rate, baseband_freq=100e3,
                            ref_level=0, y_per_div=20)
        vbox.Add (sink2.win, 1, wx.EXPAND)
        self.connect (src2, thr2, sink2)

def main ():
    app = stdgui.stdapp (test_app_flow_graph,
                         "FFT Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
