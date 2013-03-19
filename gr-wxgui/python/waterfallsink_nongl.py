#!/usr/bin/env python
#
# Copyright 2003-2005,2007,2008,2013 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, fft, filter
from gnuradio import blocks
from gnuradio import analog
from gnuradio.wxgui import stdgui2
from gnuradio.filter import window
import wx
import gnuradio.wxgui.plot as plot
import numpy
import os
import math

default_fftsink_size = (640,240)
default_fft_rate = gr.prefs().get_long('wxgui', 'fft_rate', 15)

class waterfall_sink_base(object):
    def __init__(self, input_is_real=False, baseband_freq=0,
                 sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate,
                 average=False, avg_alpha=None, title=''):

        # initialize common attributes
        self.baseband_freq = baseband_freq
        self.sample_rate = sample_rate
        self.fft_size = fft_size
        self.fft_rate = fft_rate
        self.average = average
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

    def _set_n(self):
        self.one_in_n.set_n(max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))

    def set_callback(self, callb):
        return

class waterfall_sink_f(gr.hier_block2, waterfall_sink_base):
    def __init__(self, parent, baseband_freq=0,
                 y_per_div=10, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, **kwargs):

        gr.hier_block2.__init__(self, "waterfall_sink_f",
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(0,0,0))

        waterfall_sink_base.__init__(self, input_is_real=True, baseband_freq=baseband_freq,
                               sample_rate=sample_rate, fft_size=fft_size,
                               fft_rate=fft_rate,
                               average=average, avg_alpha=avg_alpha, title=title)

        self.s2p = blocks.stream_to_vector(gr.sizeof_float, self.fft_size)
        self.one_in_n = blocks.keep_one_in_n(gr.sizeof_float * self.fft_size,
                                             max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))

        mywindow = window.blackmanharris(self.fft_size)
        self.fft = fft.fft_vfc(self.fft_size, True, mywindow)
        self.c2mag = blocks.complex_to_mag(self.fft_size)
        self.avg = filter.single_pole_iir_filter_ff(1.0, self.fft_size)
        self.log = blocks.nlog10_ff(20, self.fft_size, -20*math.log10(self.fft_size))
        self.sink = blocks.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)
	self.connect(self, self.s2p, self.one_in_n, self.fft, self.c2mag, self.avg, self.log, self.sink)

        self.win = waterfall_window(self, parent, size=size)
        self.set_average(self.average)


class waterfall_sink_c(gr.hier_block2, waterfall_sink_base):
    def __init__(self, parent, baseband_freq=0,
                 y_per_div=10, ref_level=50, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, **kwargs):

        gr.hier_block2.__init__(self, "waterfall_sink_f",
                                gr.io_signature(1, 1, gr.sizeof_gr_complex),
                                gr.io_signature(0,0,0))

        waterfall_sink_base.__init__(self, input_is_real=False, baseband_freq=baseband_freq,
                                     sample_rate=sample_rate, fft_size=fft_size,
                                     fft_rate=fft_rate,
                                     average=average, avg_alpha=avg_alpha, title=title)

        self.s2p = blocks.stream_to_vector(gr.sizeof_gr_complex, self.fft_size)
        self.one_in_n = blocks.keep_one_in_n(gr.sizeof_gr_complex * self.fft_size,
                                             max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))

        mywindow = window.blackmanharris(self.fft_size)
        self.fft = fft.fft_vcc(self.fft_size, True, mywindow)
        self.c2mag = blocks.complex_to_mag(self.fft_size)
        self.avg = filter.single_pole_iir_filter_ff(1.0, self.fft_size)
        self.log = blocks.nlog10_ff(20, self.fft_size, -20*math.log10(self.fft_size))
        self.sink = blocks.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)
	self.connect(self, self.s2p, self.one_in_n, self.fft, self.c2mag, self.avg, self.log, self.sink)

        self.win = waterfall_window(self, parent, size=size)
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

class waterfall_window (wx.Panel):
    def __init__ (self, fftsink, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        wx.Panel.__init__(self, parent, id, pos, size, style, name)
        self.set_baseband_freq = fftsink.set_baseband_freq
        self.fftsink = fftsink
        self.bm = wx.EmptyBitmap(self.fftsink.fft_size, 300, -1)

        self.scale_factor = 5.0           # FIXME should autoscale, or set this

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

        self.input_watcher = input_watcher(fftsink.msgq, fftsink.fft_size, self)


    def on_close_window (self, event):
        print "waterfall_window: on_close_window"
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


    def set_data (self, evt):
        dB = evt.data
        L = len (dB)

        dc1 = wx.MemoryDC()
        dc1.SelectObject(self.bm)
        dc1.Blit(0,1,self.fftsink.fft_size,300,dc1,0,0,wx.COPY,False,-1,-1)

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

        scale_factor = self.scale_factor
        if self.fftsink.input_is_real:     # real fft
           for x_pos in range(0, d_max):
               value = int(dB[x_pos] * scale_factor)
               value = min(255, max(0, value))
               dc1.SetPen(self.pens[value])
               dc1.DrawRectangle(x_pos*p_width, 0, p_width, 2)
        else:                               # complex fft
           for x_pos in range(0, d_max):    # positive freqs
               value = int(dB[x_pos] * scale_factor)
               value = min(255, max(0, value))
               dc1.SetPen(self.pens[value])
               dc1.DrawRectangle(x_pos*p_width + d_max, 0, p_width, 2)
           for x_pos in range(0 , d_max):   # negative freqs
               value = int(dB[x_pos+d_max] * scale_factor)
               value = min(255, max(0, value))
               dc1.SetPen(self.pens[value])
               dc1.DrawRectangle(x_pos*p_width, 0, p_width, 2)

	del dc1
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

        self.Bind(wx.EVT_MENU, self.on_average, id=self.id_average)
        #self.Bind(wx.EVT_MENU, self.on_incr_ref_level, id=self.id_incr_ref_level)
        #self.Bind(wx.EVT_MENU, self.on_decr_ref_level, id=self.id_decr_ref_level)
        #self.Bind(wx.EVT_MENU, self.on_incr_y_per_div, id=self.id_incr_y_per_div)
        #self.Bind(wx.EVT_MENU, self.on_decr_y_per_div, id=self.id_decr_y_per_div)
        #self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_1)
        #self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_2)
        #self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_5)
        #self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_10)
        #self.Bind(wx.EVT_MENU, self.on_y_per_div, id=self.id_y_per_div_20)


        # make a menu
        menu = wx.Menu()
        self.popup_menu = menu
        menu.AppendCheckItem(self.id_average, "Average")
        # menu.Append(self.id_incr_ref_level, "Incr Ref Level")
        # menu.Append(self.id_decr_ref_level, "Decr Ref Level")
        # menu.Append(self.id_incr_y_per_div, "Incr dB/div")
        # menu.Append(self.id_decr_y_per_div, "Decr dB/div")
        # menu.AppendSeparator()
        # we'd use RadioItems for these, but they're not supported on Mac
        #menu.AppendCheckItem(self.id_y_per_div_1, "1 dB/div")
        #menu.AppendCheckItem(self.id_y_per_div_2, "2 dB/div")
        #menu.AppendCheckItem(self.id_y_per_div_5, "5 dB/div")
        #menu.AppendCheckItem(self.id_y_per_div_10, "10 dB/div")
        #menu.AppendCheckItem(self.id_y_per_div_20, "20 dB/div")

        self.checkmarks = {
            self.id_average : lambda : self.fftsink.average
            #self.id_y_per_div_1 : lambda : self.fftsink.y_per_div == 1,
            #self.id_y_per_div_2 : lambda : self.fftsink.y_per_div == 2,
            #self.id_y_per_div_5 : lambda : self.fftsink.y_per_div == 5,
            #self.id_y_per_div_10 : lambda : self.fftsink.y_per_div == 10,
            #self.id_y_per_div_20 : lambda : self.fftsink.y_per_div == 20,
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

class test_top_block (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__(self, frame, panel, vbox, argv)

        fft_size = 512

        # build our flow graph
        input_rate = 20.000e3

        # Generate a complex sinusoid
        self.src1 = analog.sig_source_c(input_rate, analog.GR_SIN_WAVE, 5.75e3, 1000)
        #src1 = analog.sig_source_c(input_rate, analog.GR_CONST_WAVE, 5.75e3, 1000)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        self.thr1 = blocks.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = waterfall_sink_c(panel, title="Complex Data", fft_size=fft_size,
                                 sample_rate=input_rate, baseband_freq=100e3)
	self.connect(self.src1, self.thr1, sink1)
        vbox.Add(sink1.win, 1, wx.EXPAND)

        # generate a real sinusoid
        self.src2 = analog.sig_source_f(input_rate, analog.GR_SIN_WAVE, 5.75e3, 1000)
        self.thr2 = blocks.throttle(gr.sizeof_float, input_rate)
        sink2 = waterfall_sink_f(panel, title="Real Data", fft_size=fft_size,
                                 sample_rate=input_rate, baseband_freq=100e3)
	self.connect(self.src2, self.thr2, sink2)
        vbox.Add(sink2.win, 1, wx.EXPAND)


def main ():
    app = stdgui2.stdapp(test_top_block, "Waterfall Sink Test App")
    app.MainLoop()

if __name__ == '__main__':
    main()
