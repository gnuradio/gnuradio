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

from gnuradio import gr, gru, window
from gnuradio.wxgui import stdgui
import wx
import gnuradio.wxgui.plot as plot
import Numeric
import os
import threading
import math    

default_fftsink_size = (640,240)
default_fft_rate = gr.prefs().get_long('wxgui', 'fft_rate', 15)

class ra_waterfallsink_base(object):
    def __init__(self, input_is_real=False, baseband_freq=0,
                 sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate,
                 average=False, avg_alpha=None, title='', ofunc=None, xydfunc=None, scaling=100):

        # initialize common attributes
        self.baseband_freq = baseband_freq
        self.sample_rate = sample_rate
        self.fft_size = fft_size
        self.fft_rate = fft_rate
        self.average = average
        self.ofunc = ofunc
        self.xydfunc = xydfunc
        self.scaling = scaling
        if avg_alpha is None:
            self.avg_alpha = 2.0 / fft_rate
        else:
            self.avg_alpha = avg_alpha
        self.title = title
        self.input_is_real = input_is_real
        self.msgq = gr.msg_queue(2)         # queue up to 2 messages

    def set_average(self, average):
        self.average = average
        if average:
            self.avg.set_taps(self.avg_alpha)
        else:
            self.avg.set_taps(1.0)

    def set_avg_alpha(self, avg_alpha):
        self.avg_alpha = avg_alpha

    def set_baseband_freq(self, baseband_freq):
        self.baseband_freq = baseband_freq

    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
        self._set_n()

    def set_scaling(self,scaling):
        self.scaling = scaling
        return

    def _set_n(self):
        self.one_in_n.set_n(max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        
class ra_waterfallsink_f(gr.hier_block, ra_waterfallsink_base):
    def __init__(self, fg, parent, baseband_freq=0,
                 y_per_div=10, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', scaling=100, size=default_fftsink_size, ofunc=None, xydfunc=None):

        ra_waterfallsink_base.__init__(self, input_is_real=True, baseband_freq=baseband_freq,
                               sample_rate=sample_rate, fft_size=fft_size,
                               fft_rate=fft_rate,
                               average=average, avg_alpha=avg_alpha, title=title, ofunc=ofunc, xydfunc=xydfunc, scaling=scaling)
                               
        s2p = gr.serial_to_parallel(gr.sizeof_float, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_float * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vfc(self.fft_size, True, mywindow)
        c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)
        log = gr.nlog10_ff(20, self.fft_size, -20*math.log10(self.fft_size))
        sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)

        fg.connect (s2p, self.one_in_n, fft, c2mag, self.avg, log, sink)
        gr.hier_block.__init__(self, fg, s2p, sink)

        self.win = ra_waterfall_window(self, parent, size=size)
        self.set_average(self.average)


class ra_waterfallsink_c(gr.hier_block, ra_waterfallsink_base):
    def __init__(self, fg, parent, baseband_freq=0,
                 y_per_div=10, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, 
                 avg_alpha=None, scaling=100,
                 title='', size=default_fftsink_size, ofunc=None, xydfunc=None):

        ra_waterfallsink_base.__init__(self, input_is_real=False, baseband_freq=baseband_freq,
                                     sample_rate=sample_rate, fft_size=fft_size,
                                     fft_rate=fft_rate,
                                     average=average, 
                                     avg_alpha=avg_alpha, 
                                     title=title, ofunc=ofunc, 
                                     xydfunc=xydfunc, scaling=scaling)

        s2p = gr.serial_to_parallel(gr.sizeof_gr_complex, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_gr_complex * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))

        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vcc(self.fft_size, True, mywindow)
        c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)
        log = gr.nlog10_ff(20, self.fft_size, -20*math.log10(self.fft_size))
        sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)

        fg.connect(s2p, self.one_in_n, fft, c2mag, self.avg, log, sink)
        gr.hier_block.__init__(self, fg, s2p, sink)

        self.win = ra_waterfall_window(self, parent, size=size)
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

            complex_data = Numeric.fromstring (s, Numeric.Float32)
            de = DataEvent (complex_data)
            wx.PostEvent (self.event_receiver, de)
            del de
    

class ra_waterfall_window (wx.Panel):
    def __init__ (self, fftsink, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)

        self.fftsink = fftsink
        self.bm = wx.EmptyBitmap(1024, 512, -1)

        self.scale_factor = self.fftsink.scaling
        
        dc1 = wx.MemoryDC()
        dc1.SelectObject(self.bm)
        dc1.Clear()

        self.pens = self.make_pens()

        wx.EVT_PAINT( self, self.OnPaint )
        wx.EVT_CLOSE (self, self.on_close_window)
        EVT_DATA_EVENT (self, self.set_data)
        
        self.build_popup_menu()
        
        wx.EVT_CLOSE (self, self.on_close_window)
        self.Bind(wx.EVT_RIGHT_UP, self.on_right_click)
        self.Bind(wx.EVT_MOTION, self.on_motion)

        self.input_watcher = input_watcher(fftsink.msgq, fftsink.fft_size, self)


    def on_close_window (self, event):
        print "ra_waterfall_window: on_close_window"
        self.keep_running = False

    def const_list(self,const,len):
        return [const] * len

    def make_colormap(self):
        r = []
        r.extend(self.const_list(0,96))
        r.extend(range(0,255,4))
        r.extend(self.const_list(255,64))
        r.extend(range(255,128,-4))
        
        g = []
        g.extend(self.const_list(0,32))
        g.extend(range(0,255,4))
        g.extend(self.const_list(255,64))
        g.extend(range(255,0,-4))
        g.extend(self.const_list(0,32))
        
        b = range(128,255,4)
        b.extend(self.const_list(255,64))
        b.extend(range(255,0,-4))
        b.extend(self.const_list(0,96))
        return (r,g,b)

    def make_pens(self):
        (r,g,b) = self.make_colormap()
        pens = []
        for i in range(0,256):
            colour = wx.Colour(r[i], g[i], b[i])
            pens.append( wx.Pen(colour, 2, wx.SOLID))
        return pens
        
    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        self.DoDrawing(dc)

    def DoDrawing(self, dc=None):
        if dc is None:
            dc = wx.ClientDC(self)
        dc.DrawBitmap(self.bm, 0, 0, False )
    

    def const_list(self,const,len):
        a = [const]
        for i in range(1,len):
            a.append(const)
        return a

    def make_colormap(self):
        r = []
        r.extend(self.const_list(0,96))
        r.extend(range(0,255,4))
        r.extend(self.const_list(255,64))
        r.extend(range(255,128,-4))
        
        g = []
        g.extend(self.const_list(0,32))
        g.extend(range(0,255,4))
        g.extend(self.const_list(255,64))
        g.extend(range(255,0,-4))
        g.extend(self.const_list(0,32))
        
        b = range(128,255,4)
        b.extend(self.const_list(255,64))
        b.extend(range(255,0,-4))
        b.extend(self.const_list(0,96))
        return (r,g,b)

    def set_data (self, evt):
        dB = evt.data
        L = len (dB)

        if (self.fftsink.ofunc != None):
            self.fftsink.ofunc (evt.data, L)

        dc1 = wx.MemoryDC()
        dc1.SelectObject(self.bm)
        dc1.Blit(0,1,1024,512,dc1,0,0,wx.COPY,False,-1,-1)

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
            d_max = L/2
            p_width = 2
        else:
            d_max = L/2
            p_width = 1

        WATERFALL_WIDTH=1024
        scale_factor = self.scale_factor
        x_positions = Numeric.zeros(WATERFALL_WIDTH, Numeric.Float64)
        y_values = Numeric.zeros(WATERFALL_WIDTH, Numeric.Float64)
        x_scale = L / WATERFALL_WIDTH
        x_scale = int(x_scale)
        if self.fftsink.input_is_real:     # real fft
           for x_pos in range(0, d_max):
               value = int(dB[x_pos] * scale_factor)
               value = min(255, max(0, value))
               idx = int(x_pos/x_scale)
               idx = min(WATERFALL_WIDTH-1,idx)
               x_positions[idx] = idx
               y_values[idx] = y_values[idx] + value
        else:                               # complex fft
           for x_pos in range(0, d_max):    # positive freqs
               value = int(dB[x_pos] * scale_factor)
               value = min(255, max(0, value))
               idx = int((x_pos+d_max)/x_scale)
               idx = min(WATERFALL_WIDTH-1,idx)
               x_positions[idx] = idx
               y_values[idx] = y_values[idx] + value
           for x_pos in range(0 , d_max):   # negative freqs
               value = int(dB[x_pos+d_max] * scale_factor)
               value = min(255, max(0, value))
               idx = int((x_pos)/x_scale)
               idx = min(WATERFALL_WIDTH-1,idx)
               x_positions[idx] = idx
               y_values[idx] = y_values[idx] + value

        for i in range(0,WATERFALL_WIDTH):
            yv = y_values[i]/x_scale
            yv = int(min(255,yv))
            dc1.SetPen(self.pens[yv])
            dc1.DrawRectangle(i*p_width, 0, p_width, 1)

        self.DoDrawing (None)

    def on_average(self, evt):
        # print "on_average"
        self.fftsink.set_average(evt.IsChecked())

    def on_right_click(self, event):
        menu = self.popup_menu
        for id, pred in self.checkmarks.items():
            item = menu.FindItemById(id)
            item.Check(pred())
        self.PopupMenu(menu, event.GetPosition())

    def on_motion(self, event):
        if not self.fftsink.xydfunc == None:
            pos = event.GetPosition()
            self.fftsink.xydfunc(pos)

    def on_scaling(self, evt):
        Id = evt.GetId()
        if Id == self.id_scaling_100:
            self.fftsink.set_scaling(100)
        elif Id == self.id_scaling_150:
            self.fftsink.set_scaling(150)
        elif Id == self.id_scaling_200:
            self.fftsink.set_scaling(200)
        elif Id == self.id_scaling_250:
            self.fftsink.set_scaling(250)
        elif Id == self.id_scaling_300:
            self.fftsink.set_scaling(300)

    def build_popup_menu(self):
        self.id_scaling_100 = wx.NewId()
        self.id_scaling_150 = wx.NewId()
        self.id_scaling_200 = wx.NewId()
        self.id_scaling_250 = wx.NewId()
        self.id_scaling_300 = wx.NewId()
        self.id_average = wx.NewId()

        self.Bind(wx.EVT_MENU, self.on_average, id=self.id_average)
        self.Bind(wx.EVT_MENU, self.on_scaling, id=self.id_scaling_100)
        self.Bind(wx.EVT_MENU, self.on_scaling, id=self.id_scaling_150)
        self.Bind(wx.EVT_MENU, self.on_scaling, id=self.id_scaling_200)
        self.Bind(wx.EVT_MENU, self.on_scaling, id=self.id_scaling_250)
        self.Bind(wx.EVT_MENU, self.on_scaling, id=self.id_scaling_300)


        # make a menu
        menu = wx.Menu()
        self.popup_menu = menu
        menu.AppendCheckItem(self.id_average, "Average")
        menu.AppendCheckItem(self.id_scaling_100, "100 scale factor")
        menu.AppendCheckItem(self.id_scaling_150, "150 scale factor")
        menu.AppendCheckItem(self.id_scaling_200, "200 scale factor")
        menu.AppendCheckItem(self.id_scaling_250, "250 scale factor")
        menu.AppendCheckItem(self.id_scaling_300, "300 scale factor")

        self.checkmarks = {
            self.id_average : lambda : self.fftsink.average,
            self.id_scaling_100 : lambda : self.fftsink.scaling == 100,
            self.id_scaling_150 : lambda : self.fftsink.scaling == 150,
            self.id_scaling_200 : lambda : self.fftsink.scaling == 200,
            self.id_scaling_250 : lambda : self.fftsink.scaling == 250,
            self.id_scaling_300 : lambda : self.fftsink.scaling == 300,
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

def make_ra_waterfallsink_f(fg, parent, title, fft_size, input_rate):
    
    block = ra_waterfallsink_f(fg, parent, title=title, fft_size=fft_size,
                             sample_rate=input_rate)
    return (block, block.win)

# returns (block, win).
#   block requires a single input stream of gr_complex
#   win is a subclass of wxWindow

def make_ra_waterfallsink_c(fg, parent, title, fft_size, input_rate):
    block = ra_waterfallsink_c(fg, parent, title=title, fft_size=fft_size,
                             sample_rate=input_rate)
    return (block, block.win)


# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

class test_app_flow_graph (stdgui.gui_flow_graph):
    def __init__(self, frame, panel, vbox, argv):
        stdgui.gui_flow_graph.__init__ (self, frame, panel, vbox, argv)

        fft_size = 512

        # build our flow graph
        input_rate = 20.000e3

        # Generate a complex sinusoid
        src1 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 5.75e3, 1000)
        #src1 = gr.sig_source_c (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1000)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr1 = gr.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = ra_waterfallsink_c (self, panel, title="Complex Data", fft_size=fft_size,
                                  sample_rate=input_rate, baseband_freq=100e3)
        vbox.Add (sink1.win, 1, wx.EXPAND)
        self.connect (src1, thr1, sink1)

        # generate a real sinusoid
        src2 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 5.75e3, 1000)
        #src2 = gr.sig_source_f (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1000)
        thr2 = gr.throttle(gr.sizeof_float, input_rate)
        sink2 = ra_waterfallsink_f (self, panel, title="Real Data", fft_size=fft_size,
                                  sample_rate=input_rate, baseband_freq=100e3)
        vbox.Add (sink2.win, 1, wx.EXPAND)
        self.connect (src2, thr2, sink2)

def main ():
    app = stdgui.stdapp (test_app_flow_graph,
                         "Waterfall Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
