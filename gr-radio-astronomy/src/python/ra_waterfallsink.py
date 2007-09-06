#!/usr/bin/env python
#
# Copyright 2003,2004,2005 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
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
import os
import threading
import math    

default_fftsink_size = (640,240)
default_fft_rate = gr.prefs().get_long('wxgui', 'fft_rate', 15)

def axis_design( x1, x2, nx ):
    # Given start, end, and number of labels, return value of first label,
    # increment between labels, number of unlabeled division between labels,
    # and scale factor.

    dx = abs( x2 - x1 )/float(nx+1)  # allow for space at each end
    ldx = math.log10(dx)
    l2 = math.log10(2.)
    l5 = math.log10(5.)
    le = math.floor(ldx)
    lf = ldx - le
    if lf < l2/2:
        c = 1
        dt = 10
    elif lf < (l2+l5)/2:
        c = 2
        dt = 4
    elif lf < (l5+1)/2:
        c = 5
        dt = 5
    else:
        c = 1
        dt = 10
        le += 1
    inc = c*pow( 10., le )
    first = math.ceil( x1/inc )*inc
    scale = 1.
    while ( abs(x1*scale) >= 1e5 ) or ( abs(x2*scale) >= 1e5 ):
        scale *= 1e-3
    return ( first, inc, dt, scale )
    

class waterfall_sink_base(object):
    def __init__(self, input_is_real=False, baseband_freq=0,
                 sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate,
                 average=False, avg_alpha=None, title='', ofunc=None, xydfunc=None):

        # initialize common attributes
        self.baseband_freq = baseband_freq
        self.sample_rate = sample_rate
        self.fft_size = fft_size
        self.fft_rate = fft_rate
        self.average = average
        self.ofunc = ofunc
        self.xydfunc = xydfunc
        if avg_alpha is None:
            self.avg_alpha = 2.0 / fft_rate
        else:
            self.avg_alpha = avg_alpha
        self.title = title
        self.input_is_real = input_is_real
        self.msgq = gr.msg_queue(2)         # queue up to 2 messages

    def reconnect( self, fg ):
        fg.connect( *self.block_list )

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
        
class waterfall_sink_f(gr.hier_block, waterfall_sink_base):
    def __init__(self, fg, parent, baseband_freq=0,
                 ref_level=0, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None,
                 title='', size=default_fftsink_size, report=None, span=40, ofunc=None, xydfunc=None):

        waterfall_sink_base.__init__(self, input_is_real=True,
                                     baseband_freq=baseband_freq,
                                     sample_rate=sample_rate,
                                     fft_size=fft_size, fft_rate=fft_rate,
                                     average=average, avg_alpha=avg_alpha,
                                     title=title)
                               
        s2p = gr.serial_to_parallel(gr.sizeof_float, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_float * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))
        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vfc(self.fft_size, True, mywindow)
        c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)
        log = gr.nlog10_ff(20, self.fft_size, -20*math.log10(self.fft_size))
        sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)

        self.block_list = (s2p, self.one_in_n, fft, c2mag, self.avg, log, sink)
        self.reconnect( fg )
        gr.hier_block.__init__(self, fg, s2p, sink)

        self.win = waterfall_window(self, parent, size=size, report=report,
                                    ref_level=ref_level, span=span, ofunc=ofunc, xydfunc=xydfunc)
        self.set_average(self.average)


class waterfall_sink_c(gr.hier_block, waterfall_sink_base):
    def __init__(self, fg, parent, baseband_freq=0,
                 ref_level=0, sample_rate=1, fft_size=512,
                 fft_rate=default_fft_rate, average=False, avg_alpha=None, 
                 title='', size=default_fftsink_size, report=None, span=40, ofunc=None, xydfunc=None):

        waterfall_sink_base.__init__(self, input_is_real=False,
                                     baseband_freq=baseband_freq,
                                     sample_rate=sample_rate,
                                     fft_size=fft_size,
                                     fft_rate=fft_rate,
                                     average=average, avg_alpha=avg_alpha,
                                     title=title)

        s2p = gr.serial_to_parallel(gr.sizeof_gr_complex, self.fft_size)
        self.one_in_n = gr.keep_one_in_n(gr.sizeof_gr_complex * self.fft_size,
                                         max(1, int(self.sample_rate/self.fft_size/self.fft_rate)))

        mywindow = window.blackmanharris(self.fft_size)
        fft = gr.fft_vcc(self.fft_size, True, mywindow)
        c2mag = gr.complex_to_mag(self.fft_size)
        self.avg = gr.single_pole_iir_filter_ff(1.0, self.fft_size)
        log = gr.nlog10_ff(20, self.fft_size, -20*math.log10(self.fft_size))
        sink = gr.message_sink(gr.sizeof_float * self.fft_size, self.msgq, True)

        self.block_list = (s2p, self.one_in_n, fft, c2mag, self.avg, log, sink)
        self.reconnect( fg )
        gr.hier_block.__init__(self, fg, s2p, sink)

        self.win = waterfall_window(self, parent, size=size, report=report,
                                    ref_level=ref_level, span=span, ofunc=ofunc, xydfunc=xydfunc)
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
    

class waterfall_window (wx.ScrolledWindow):
    def __init__ (self, fftsink, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize,
                  style = wx.DEFAULT_FRAME_STYLE, name = "", report=None,
                  ref_level = 0, span = 50, ofunc=None, xydfunc=None):
        wx.ScrolledWindow.__init__(self, parent, id, pos, size,
                                   style|wx.HSCROLL, name)
        self.parent = parent
        self.SetCursor(wx.StockCursor(wx.CURSOR_IBEAM))
        self.ref_level = ref_level
        self.scale_factor = 256./span

        self.ppsh = 128  # pixels per scroll, horizontal
        self.SetScrollbars( self.ppsh, 0, fftsink.fft_size/self.ppsh, 0 )

        self.fftsink = fftsink
        self.size = size
        self.report = report
        self.ofunc = ofunc
        self.xydfunc = xydfunc

        dc1 = wx.MemoryDC()
        dc1.SetFont( wx.SMALL_FONT )
        self.h_scale = dc1.GetCharHeight() + 3
        #self.bm_size = ( self.fftsink.fft_size, self.size[1] - self.h_scale )
        self.im_size = ( self.fftsink.fft_size, self.size[1] - self.h_scale )
        #self.bm = wx.EmptyBitmap( self.bm_size[0], self.bm_size[1], -1)
        self.im = wx.EmptyImage( self.im_size[0], self.im_size[1], True )
        self.im_cur = 0

        self.baseband_freq = None

        self.make_pens()

        wx.EVT_PAINT( self, self.OnPaint )
        wx.EVT_CLOSE (self, self.on_close_window)
        #wx.EVT_LEFT_UP(self, self.on_left_up)
        #wx.EVT_LEFT_DOWN(self, self.on_left_down)
        EVT_DATA_EVENT (self, self.set_data)
        
        self.build_popup_menu()
        
        wx.EVT_CLOSE (self, self.on_close_window)
        self.Bind(wx.EVT_RIGHT_UP, self.on_right_click)
        self.Bind(wx.EVT_MOTION, self.on_motion)

        self.down_pos = None

        self.input_watcher = input_watcher(fftsink.msgq, fftsink.fft_size, self)

    def on_close_window (self, event):
        self.keep_running = False

    def on_left_down( self, evt ):
        self.down_pos = evt.GetPosition()
        self.down_time = evt.GetTimestamp()

    def on_left_up( self, evt ):
        if self.down_pos:
            dt = ( evt.GetTimestamp() - self.down_time )/1000.
            pph = self.fftsink.fft_size/float(self.fftsink.sample_rate)
            dx =  evt.GetPosition()[0] - self.down_pos[0]
            if dx != 0:
                rt = pph/dx
            else:
                rt = 0
            t = 'Down time: %f  Delta f: %f  Period: %f' % ( dt, dx/pph, rt )
            print t
            if self.report:
                self.report(t)

    def on_motion(self, event):
        if self.xydfunc:
            pos = event.GetPosition()
            self.xydfunc(pos)


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
        self.rgb = numpy.transpose( numpy.array( (r,g,b) ).astype(numpy.int8) )
        
    def OnPaint(self, event):
        dc = wx.BufferedPaintDC(self)
        self.DoDrawing( dc )

    def DoDrawing(self,dc):
        w, h = self.GetClientSizeTuple()
        w = min( w, self.fftsink.fft_size )
        if w <= 0:
            return

        if dc is None:
            dc = wx.BufferedDC( wx.ClientDC(self), (w,h) )

        dc.SetBackground( wx.Brush( self.GetBackgroundColour(), wx.SOLID ) )
        dc.Clear()

        x, y = self.GetViewStart()
        x *= self.ppsh

        ih = min( h - self.h_scale, self.im_size[1] - self.im_cur )
        r = wx.Rect( x, self.im_cur, w, ih )
        bm = wx.BitmapFromImage( self.im.GetSubImage(r) )
        dc.DrawBitmap( bm, 0, self.h_scale )
        rem = min( self.im_size[1] - ih, h - ih - self.h_scale )
        if( rem > 0 ):
            r = wx.Rect( x, 0, w, rem )
            bm = wx.BitmapFromImage( self.im.GetSubImage(r) )
            dc.DrawBitmap( bm, 0, ih + self.h_scale )
        
        # Draw axis
        if self.baseband_freq != self.fftsink.baseband_freq:
            self.baseband_freq = self.fftsink.baseband_freq
            t = self.fftsink.sample_rate*w/float(self.fftsink.fft_size)
            self.ax_spec = axis_design( self.baseband_freq - t/2,
                                        self.baseband_freq + t/2, 7 )
        dc.SetFont( wx.SMALL_FONT )
        fo = self.baseband_freq
        po = self.fftsink.fft_size/2
        pph = self.fftsink.fft_size/float(self.fftsink.sample_rate)
        f = math.floor((fo-po/pph)/self.ax_spec[1])*self.ax_spec[1]
        while True:
            t = po + ( f - fo )*pph
            s = str( f*self.ax_spec[3] )
            e = dc.GetTextExtent( s )
            if t - e[1]/2 >= x + w:
                break
            dc.DrawText( s, t - x - e[0]/2, 0 )
            dc.DrawLine( t - x, e[1] - 1, t - x, self.h_scale )
            dt = self.ax_spec[1]/self.ax_spec[2]*pph
            for i in range(self.ax_spec[2]-1):
                t += dt
                if t >= x + w:
                    break
                dc.DrawLine( t - x, e[1] + 1, t - x, self.h_scale )
            f += self.ax_spec[1]

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

        if self.ofunc != None:
            self.ofunc(evt.data, L)
        #dc1 = wx.MemoryDC()
        #dc1.SelectObject(self.bm)

        # Scroll existing bitmap
        if 1:
            #dc1.Blit(0,1,self.bm_size[0],self.bm_size[1]-1,dc1,0,0,
            #         wx.COPY,False,-1,-1)
            pass
        else:
            for i in range( self.bm_size[1]-1, 0, -1 ):
                dc1.Blit( 0, i, self.bm_size[0], 1, dc1, 0, i-1 )

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
        dB -= self.ref_level
        dB *= scale_factor
        dB = dB.astype(numpy.int_).clip( min=0, max=255 )
        if self.fftsink.input_is_real:     # real fft
            dB = numpy.array( ( dB[0:d_max][::-1], dB[0:d_max] ) )
        else:                               # complex fft
            dB = numpy.concatenate( ( dB[d_max:L], dB[0:d_max] ) )

        dB = self.rgb[dB]
        img = wx.ImageFromData( L, 1, dB.ravel().tostring() )
        #bm = wx.BitmapFromImage( img )
        #dc1.DrawBitmap( bm, 0, 0 )
        ibuf = self.im.GetDataBuffer()
        self.im_cur -= 1
        if self.im_cur < 0:
            self.im_cur = self.im_size[1] - 1
        start = 3*self.im_cur*self.im_size[0]
        ibuf[start:start+3*self.im_size[0]] = img.GetData()

        #del dc1
        self.DoDrawing(None)

    def on_average(self, evt):
        # print "on_average"
        self.fftsink.set_average(evt.IsChecked())

    def on_right_click(self, event):
        menu = self.popup_menu
        self.PopupMenu(menu, event.GetPosition())


    def build_popup_menu(self):
        id_ref_gain = wx.NewId()
        self.Bind( wx.EVT_MENU, self.on_ref_gain, id=id_ref_gain )

        # make a menu
        menu = wx.Menu()
        self.popup_menu = menu
        menu.Append( id_ref_gain, "Ref Level and Gain" )
        self.rg_dialog = None

        self.checkmarks = {
            #self.id_average : lambda : self.fftsink.average
            }

    def on_ref_gain( self, evt ):
        if self.rg_dialog == None:
            self.rg_dialog = rg_dialog( self.parent, self.set_ref_gain,
                                        ref=self.ref_level,
                                        span=256./self.scale_factor )
        self.rg_dialog.Show( True )

    def set_ref_gain( self, ref, span ):
        self.ref_level = ref
        self.scale_factor = 256/span

class rg_dialog( wx.Dialog ):
    def __init__( self, parent, set_function, ref=0, span=256./5. ):
        wx.Dialog.__init__( self, parent, -1, "Waterfall Settings" )
        self.set_function = set_function
        #status_bar = wx.StatusBar( self, -1 )

        d_sizer = wx.BoxSizer( wx.VERTICAL )  # dialog sizer
        f_sizer = wx.BoxSizer( wx.VERTICAL )  # form sizer
        vs = 10

        #f_sizer.Add( fn_sizer, 0, flag=wx.TOP, border=10 )

        h_sizer = wx.BoxSizer( wx.HORIZONTAL )
        self.ref = tab_item( self, "Ref Level:", 4, "dB" )
        self.ref.ctrl.SetValue( "%d" % ref )
        h_sizer.Add((0,0),1)
        h_sizer.Add( self.ref, 0 )
        h_sizer.Add((0,0),1)
        self.span = tab_item( self, "Range:", 4, "dB" )
        self.span.ctrl.SetValue( "%d" % span )
        h_sizer.Add( self.span, 0 )
        h_sizer.Add((0,0),1)
        f_sizer.Add( h_sizer, 0, flag=wx.TOP|wx.EXPAND, border=vs )

        d_sizer.Add((0,0),1)
        d_sizer.Add( f_sizer, 0, flag=wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND )
        d_sizer.Add((0,0),1)
        d_sizer.Add((0,0),1)

        button_sizer = wx.BoxSizer( wx.HORIZONTAL )
        apply_button = wx.Button( self, -1, "Apply" )
        apply_button.Bind( wx.EVT_BUTTON, self.apply_evt )
        cancel_button = wx.Button( self, -1, "Cancel" )
        cancel_button.Bind( wx.EVT_BUTTON, self.cancel_evt )
        ok_button = wx.Button( self, -1, "OK" )
        ok_button.Bind( wx.EVT_BUTTON, self.ok_evt )
        button_sizer.Add((0,0),1)
        button_sizer.Add( apply_button, 0,
                          flag=wx.ALIGN_CENTER_HORIZONTAL )
        button_sizer.Add((0,0),1)
        button_sizer.Add( cancel_button, 0,
                          flag=wx.ALIGN_CENTER_HORIZONTAL )
        button_sizer.Add((0,0),1)
        button_sizer.Add( ok_button, 0,
                          flag=wx.ALIGN_CENTER_HORIZONTAL )
        button_sizer.Add((0,0),1)
        d_sizer.Add( button_sizer, 0,
                     flag=wx.EXPAND|wx.ALIGN_CENTER|wx.BOTTOM, border=30 )
        self.SetSizer( d_sizer )

    def apply_evt( self, evt ):
        self.do_apply()

    def cancel_evt( self, evt ):
        self.Show( False )

    def ok_evt( self, evt ):
        self.do_apply()
        self.Show( False )

    def do_apply( self ):
        r = float( self.ref.ctrl.GetValue() )
        g = float( self.span.ctrl.GetValue() )
        self.set_function( r, g )

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

# One of many copies that should be consolidated . . .
def tab_item( parent, label, chars, units, style=wx.TE_RIGHT, value="" ):
    s = wx.BoxSizer( wx.HORIZONTAL )
    s.Add( wx.StaticText( parent, -1, label ), 0,
           flag=wx.ALIGN_CENTER_VERTICAL )
    s.ctrl = wx.TextCtrl( parent, -1, style=style, value=value )
    s.ctrl.SetMinSize( ( (1.00+chars)*s.ctrl.GetCharWidth(),
                                 1.25*s.ctrl.GetCharHeight() ) )
    s.Add( s.ctrl, -1, flag=wx.LEFT, border=3 )
    s.Add( wx.StaticText( parent, -1, units ), 0,
           flag=wx.ALIGN_CENTER_VERTICAL|wx.LEFT, border=1 )
    return s


# ----------------------------------------------------------------
#          	      Deprecated interfaces
# ----------------------------------------------------------------

# returns (block, win).
#   block requires a single input stream of float
#   win is a subclass of wxWindow

def make_waterfall_sink_f(fg, parent, title, fft_size, input_rate):
    
    block = waterfall_sink_f(fg, parent, title=title, fft_size=fft_size,
                             sample_rate=input_rate)
    return (block, block.win)

# returns (block, win).
#   block requires a single input stream of gr_complex
#   win is a subclass of wxWindow

def make_waterfall_sink_c(fg, parent, title, fft_size, input_rate):
    block = waterfall_sink_c(fg, parent, title=title, fft_size=fft_size,
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

        sink1 = waterfall_sink_c (self, panel, title="Complex Data",
                                  fft_size=fft_size,
                                  sample_rate=input_rate, baseband_freq=0,
                                  size=(600,144) )
        vbox.Add (sink1.win, 1, wx.EXPAND)
        self.connect (src1, thr1, sink1)

        # generate a real sinusoid
        src2 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 5.75e3, 1000)
        #src2 = gr.sig_source_f (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1000)
        thr2 = gr.throttle(gr.sizeof_float, input_rate)
        sink2 = waterfall_sink_f (self, panel, title="Real Data", fft_size=fft_size,
                                  sample_rate=input_rate, baseband_freq=0)
        vbox.Add (sink2.win, 1, wx.EXPAND)
        self.connect (src2, thr2, sink2)

def main ():
    app = stdgui.stdapp (test_app_flow_graph,
                         "Waterfall Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
