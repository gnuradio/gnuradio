#!/usr/bin/env python
#
# Copyright 2003,2004,2006,2007 Free Software Foundation, Inc.
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

from gnuradio import gr, gru, eng_notation
from gnuradio.wxgui import stdgui2
import wx
import gnuradio.wxgui.plot as plot
import numpy
import struct

default_scopesink_size = (640, 240)
default_v_scale = 1000
default_frame_decim = gr.prefs().get_long('wxgui', 'frame_decim', 1)

class scope_sink_f(gr.hier_block2):
    def __init__(self, parent, title='', sample_rate=1,
                 size=default_scopesink_size, frame_decim=default_frame_decim,
                 v_scale=default_v_scale, t_scale=None, num_inputs=1, **kwargs):

        gr.hier_block2.__init__(self, "scope_sink_f",
                                gr.io_signature(num_inputs, num_inputs, gr.sizeof_float),
                                gr.io_signature(0,0,0))

        msgq = gr.msg_queue(2)         # message queue that holds at most 2 messages
        self.guts = gr.oscope_sink_f(sample_rate, msgq)
        for i in range(num_inputs):        
          self.connect((self, i), (self.guts, i))

        self.win = scope_window(win_info (msgq, sample_rate, frame_decim,
                                          v_scale, t_scale, self.guts, title), parent)

    def set_sample_rate(self, sample_rate):
        self.guts.set_sample_rate(sample_rate)
        self.win.info.set_sample_rate(sample_rate)

class scope_sink_c(gr.hier_block2):
    def __init__(self, parent, title='', sample_rate=1,
                 size=default_scopesink_size, frame_decim=default_frame_decim,
                 v_scale=default_v_scale, t_scale=None, num_inputs=1, xy_mode=False, **kwargs):

        gr.hier_block2.__init__(self, "scope_sink_c",
                                gr.io_signature(num_inputs, num_inputs, gr.sizeof_gr_complex),
                                gr.io_signature(0,0,0))

        msgq = gr.msg_queue(2)         # message queue that holds at most 2 messages
        self.guts = gr.oscope_sink_f(sample_rate, msgq)
        for i in range(num_inputs):      
	        c2f = gr.complex_to_float()  
	        self.connect((self, i), c2f)
	        self.connect((c2f, 0), (self.guts, 2*i+0))
	        self.connect((c2f, 1), (self.guts, 2*i+1))
        
        self.win = scope_window(win_info(msgq, sample_rate, frame_decim,
                                         v_scale, t_scale, self.guts, title), parent)
        self.win.info.xy = xy_mode
        
    def set_sample_rate(self, sample_rate):
        self.guts.set_sample_rate(sample_rate)
        self.win.info.set_sample_rate(sample_rate)

class constellation_sink(scope_sink_c):
    def __init__(self, parent, title='Constellation', sample_rate=1,
                 size=default_scopesink_size, frame_decim=default_frame_decim):
        scope_sink_c.__init__(self, parent=parent, title=title, sample_rate=sample_rate,
                 size=size, frame_decim=frame_decim)
        self.win.info.xy = True	#constellation mode

# ========================================================================


time_base_list = [                      # time / division
    1.0e-7,   # 100ns / div
    2.5e-7,
    5.0e-7,
    1.0e-6,   #   1us / div
    2.5e-6,
    5.0e-6,
    1.0e-5,   #  10us / div
    2.5e-5,
    5.0e-5,
    1.0e-4,   # 100us / div
    2.5e-4,
    5.0e-4,
    1.0e-3,   #   1ms / div
    2.5e-3,
    5.0e-3,
    1.0e-2,   #  10ms / div
    2.5e-2,
    5.0e-2
    ]

v_scale_list = [ # counts / div, LARGER gains are SMALLER /div, appear EARLIER
    2.0e-3,   # 2m / div, don't call it V/div it's actually counts/div
    5.0e-3,
    1.0e-2,
    2.0e-2,
    5.0e-2,
    1.0e-1,
    2.0e-1,
    5.0e-1,
    1.0e+0,
    2.0e+0,
    5.0e+0,
    1.0e+1,
    2.0e+1,
    5.0e+1,
    1.0e+2,
    2.0e+2,
    5.0e+2,
    1.0e+3,
    2.0e+3,
    5.0e+3,
    1.0e+4 # 10000 /div, USRP full scale is -/+ 32767
    ]

    
wxDATA_EVENT = wx.NewEventType()

def EVT_DATA_EVENT(win, func):
    win.Connect(-1, -1, wxDATA_EVENT, func)

class DataEvent(wx.PyEvent):
    def __init__(self, data):
        wx.PyEvent.__init__(self)
        self.SetEventType (wxDATA_EVENT)
        self.data = data

    def Clone (self): 
        self.__class__ (self.GetId())


class win_info (object):
    __slots__ = ['msgq', 'sample_rate', 'frame_decim', 'v_scale', 
                 'scopesink', 'title',
                 'time_scale_cursor', 'v_scale_cursor', 'marker', 'xy',
                 'autorange', 'running']

    def __init__ (self, msgq, sample_rate, frame_decim, v_scale, t_scale,
                  scopesink, title = "Oscilloscope", xy=False):
        self.msgq = msgq
        self.sample_rate = sample_rate
        self.frame_decim = frame_decim
        self.scopesink = scopesink
        self.title = title;

        self.time_scale_cursor = gru.seq_with_cursor(time_base_list, initial_value = t_scale)
        self.v_scale_cursor = gru.seq_with_cursor(v_scale_list, initial_value = v_scale)

        self.marker = 'line'
        self.xy = xy
        self.autorange = not v_scale
        self.running = True

    def get_time_per_div (self):
        return self.time_scale_cursor.current ()

    def get_volts_per_div (self):
        return self.v_scale_cursor.current ()

    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
        
    def get_sample_rate (self):
        return self.sample_rate

    def get_decimation_rate (self):
        return 1.0

    def set_marker (self, s):
        self.marker = s

    def get_marker (self):
        return self.marker


class input_watcher (gru.msgq_runner):
    def __init__ (self, msgq, event_receiver, frame_decim, **kwds):
        self.event_receiver = event_receiver
        self.frame_decim = frame_decim
        self.iscan = 0
        gru.msgq_runner.__init__(self, msgq, self.handle_msg)

    def handle_msg(self, msg):
        if self.iscan == 0:            # only display at frame_decim
            self.iscan = self.frame_decim
            
            nchan = int(msg.arg1())    # number of channels of data in msg
            nsamples = int(msg.arg2()) # number of samples in each channel
            
            s = msg.to_string()      # get the body of the msg as a string
            
            bytes_per_chan = nsamples * gr.sizeof_float
            
            records = []
            for ch in range (nchan):
                
                start = ch * bytes_per_chan
                chan_data = s[start:start+bytes_per_chan]
                rec = numpy.fromstring (chan_data, numpy.float32)
                records.append (rec)
            
            # print "nrecords = %d, reclen = %d" % (len (records),nsamples)
            
            de = DataEvent (records)
            wx.PostEvent (self.event_receiver, de)
            records = []
            del de

        self.iscan -= 1
    

class scope_window (wx.Panel):

    def __init__ (self, info, parent, id = -1,
                  pos = wx.DefaultPosition, size = wx.DefaultSize, name = ""):
        wx.Panel.__init__ (self, parent, -1)
        self.info = info

        vbox = wx.BoxSizer (wx.VERTICAL)

        self.graph = graph_window (info, self, -1)

        vbox.Add (self.graph, 1, wx.EXPAND)
        vbox.Add (self.make_control_box(), 0, wx.EXPAND)
        vbox.Add (self.make_control2_box(), 0, wx.EXPAND)

        self.sizer = vbox
        self.SetSizer (self.sizer)
        self.SetAutoLayout (True)
        self.sizer.Fit (self)
        self.set_autorange(self.info.autorange)
        

    # second row of control buttons etc. appears BELOW control_box
    def make_control2_box (self):
        ctrlbox = wx.BoxSizer (wx.HORIZONTAL)

        self.inc_v_button = wx.Button (self, 1101, " < ", style=wx.BU_EXACTFIT)
        self.inc_v_button.SetToolTipString ("Increase vertical range")
        wx.EVT_BUTTON (self, 1101, self.incr_v_scale) # ID matches button ID above

        self.dec_v_button  = wx.Button (self, 1100, " > ", style=wx.BU_EXACTFIT)
        self.dec_v_button.SetToolTipString ("Decrease vertical range")
        wx.EVT_BUTTON (self, 1100, self.decr_v_scale)

        self.v_scale_label = wx.StaticText (self, 1002, "None") # vertical /div
        self.update_v_scale_label ()

        self.autorange_checkbox = wx.CheckBox (self, 1102, "Autorange")
        self.autorange_checkbox.SetToolTipString ("Select autorange on/off")
        wx.EVT_CHECKBOX(self, 1102, self.autorange_checkbox_event)

        ctrlbox.Add ((5,0) ,0) # left margin space
        ctrlbox.Add (self.inc_v_button, 0, wx.EXPAND)
        ctrlbox.Add (self.dec_v_button, 0, wx.EXPAND)
        ctrlbox.Add (self.v_scale_label, 0, wx.ALIGN_CENTER)
        ctrlbox.Add ((20,0) ,0) # spacer
        ctrlbox.Add (self.autorange_checkbox, 0, wx.ALIGN_CENTER)

        return ctrlbox

    def make_control_box (self):
        ctrlbox = wx.BoxSizer (wx.HORIZONTAL)

        tb_left = wx.Button (self, 1001, " < ", style=wx.BU_EXACTFIT)
        tb_left.SetToolTipString ("Increase time base")
        wx.EVT_BUTTON (self, 1001, self.incr_timebase)


        tb_right  = wx.Button (self, 1000, " > ", style=wx.BU_EXACTFIT)
        tb_right.SetToolTipString ("Decrease time base")
        wx.EVT_BUTTON (self, 1000, self.decr_timebase)

        self.time_base_label = wx.StaticText (self, 1002, "")
        self.update_timebase_label ()

        ctrlbox.Add ((5,0) ,0)
        # ctrlbox.Add (wx.StaticText (self, -1, "Horiz Scale: "), 0, wx.ALIGN_CENTER)
        ctrlbox.Add (tb_left, 0, wx.EXPAND)
        ctrlbox.Add (tb_right, 0, wx.EXPAND)
        ctrlbox.Add (self.time_base_label, 0, wx.ALIGN_CENTER)

        ctrlbox.Add ((10,0) ,1)            # stretchy space

        ctrlbox.Add (wx.StaticText (self, -1, "Trig: "), 0, wx.ALIGN_CENTER)
        self.trig_chan_choice = wx.Choice (self, 1004,
                                           choices = ['Ch1', 'Ch2', 'Ch3', 'Ch4'])
        self.trig_chan_choice.SetToolTipString ("Select channel for trigger")
        wx.EVT_CHOICE (self, 1004, self.trig_chan_choice_event)
        ctrlbox.Add (self.trig_chan_choice, 0, wx.ALIGN_CENTER)

        self.trig_mode_choice = wx.Choice (self, 1005,
                                           choices = ['Free', 'Auto', 'Norm'])
        self.trig_mode_choice.SetSelection(1)
        self.trig_mode_choice.SetToolTipString ("Select trigger slope or Auto (untriggered roll)")
        wx.EVT_CHOICE (self, 1005, self.trig_mode_choice_event)
        ctrlbox.Add (self.trig_mode_choice, 0, wx.ALIGN_CENTER)

        trig_level50 = wx.Button (self, 1006, "50%")
        trig_level50.SetToolTipString ("Set trigger level to 50%")
        wx.EVT_BUTTON (self, 1006, self.set_trig_level50)
        ctrlbox.Add (trig_level50, 0, wx.EXPAND)

        run_stop = wx.Button (self, 1007, "Run/Stop")
        run_stop.SetToolTipString ("Toggle Run/Stop mode")
        wx.EVT_BUTTON (self, 1007, self.run_stop)
        ctrlbox.Add (run_stop, 0, wx.EXPAND)

        ctrlbox.Add ((10, 0) ,1)            # stretchy space

        ctrlbox.Add (wx.StaticText (self, -1, "Fmt: "), 0, wx.ALIGN_CENTER)
        self.marker_choice = wx.Choice (self, 1002, choices = self._marker_choices)
        self.marker_choice.SetToolTipString ("Select plotting with lines, pluses or dots")
        wx.EVT_CHOICE (self, 1002, self.marker_choice_event)
        ctrlbox.Add (self.marker_choice, 0, wx.ALIGN_CENTER)

        self.xy_choice = wx.Choice (self, 1003, choices = ['X:t', 'X:Y'])
        self.xy_choice.SetToolTipString ("Select X vs time or X vs Y display")
        wx.EVT_CHOICE (self, 1003, self.xy_choice_event)
        ctrlbox.Add (self.xy_choice, 0, wx.ALIGN_CENTER)

        return ctrlbox
    
    _marker_choices = ['line', 'plus', 'dot']

    def update_timebase_label (self):
        time_per_div = self.info.get_time_per_div ()
        s = ' ' + eng_notation.num_to_str (time_per_div) + 's/div'
        self.time_base_label.SetLabel (s)
        
    def decr_timebase (self, evt):
        self.info.time_scale_cursor.prev ()
        self.update_timebase_label ()

    def incr_timebase (self, evt):
        self.info.time_scale_cursor.next ()
        self.update_timebase_label ()

    def update_v_scale_label (self):
        volts_per_div = self.info.get_volts_per_div ()
        s = ' ' + eng_notation.num_to_str (volts_per_div) + '/div' # Not V/div
        self.v_scale_label.SetLabel (s)
        
    def decr_v_scale (self, evt):
        self.info.v_scale_cursor.prev ()
        self.update_v_scale_label ()

    def incr_v_scale (self, evt):
        self.info.v_scale_cursor.next ()
        self.update_v_scale_label ()
        
    def marker_choice_event (self, evt):
        s = evt.GetString ()
        self.set_marker (s)

    def set_autorange(self, on):
        if on:
            self.v_scale_label.SetLabel(" (auto)")
            self.info.autorange = True
            self.autorange_checkbox.SetValue(True)
            self.inc_v_button.Enable(False)
            self.dec_v_button.Enable(False)
        else:
            if self.graph.y_range:
                (l,u) = self.graph.y_range # found by autorange
                self.info.v_scale_cursor.set_index_by_value((u-l)/8.0)
            self.update_v_scale_label()
            self.info.autorange = False
            self.autorange_checkbox.SetValue(False)
            self.inc_v_button.Enable(True)
            self.dec_v_button.Enable(True)
            
    def autorange_checkbox_event(self, evt):
        if evt.Checked():
            self.set_autorange(True)
        else:
            self.set_autorange(False)
            
    def set_marker (self, s):
        self.info.set_marker (s)        # set info for drawing routines
        i = self.marker_choice.FindString (s)
        assert i >= 0, "Hmmm, set_marker problem"
        self.marker_choice.SetSelection (i)

    def set_format_line (self):
        self.set_marker ('line')

    def set_format_dot (self):
        self.set_marker ('dot')

    def set_format_plus (self):
        self.set_marker ('plus')
        
    def xy_choice_event (self, evt):
        s = evt.GetString ()
        self.info.xy = s == 'X:Y'

    def trig_chan_choice_event (self, evt):
        s = evt.GetString ()
        ch = int (s[-1]) - 1
        self.info.scopesink.set_trigger_channel (ch)

    def trig_mode_choice_event (self, evt):
        sink = self.info.scopesink
        s = evt.GetString ()
        if s == 'Norm':
            sink.set_trigger_mode (gr.gr_TRIG_MODE_NORM)
        elif s == 'Auto':
            sink.set_trigger_mode (gr.gr_TRIG_MODE_AUTO)
        elif s == 'Free':
            sink.set_trigger_mode (gr.gr_TRIG_MODE_FREE)
        else:
            assert 0, "Bad trig_mode_choice string"
    
    def set_trig_level50 (self, evt):
        self.info.scopesink.set_trigger_level_auto ()

    def run_stop (self, evt):
        self.info.running = not self.info.running
        

class graph_window (plot.PlotCanvas):

    channel_colors = ['BLUE', 'RED',
                      'CYAN', 'MAGENTA', 'GREEN', 'YELLOW']
    
    def __init__ (self, info, parent, id = -1,
                  pos = wx.DefaultPosition, size = (640, 240),
                  style = wx.DEFAULT_FRAME_STYLE, name = ""):
        plot.PlotCanvas.__init__ (self, parent, id, pos, size, style, name)

        self.SetXUseScopeTicks (True)
        self.SetEnableGrid (True)
        self.SetEnableZoom (True)
        self.SetEnableLegend(True)
        # self.SetBackgroundColour ('black')
        
        self.info = info;
        self.y_range = None
        self.x_range = None
        self.avg_y_min = None
        self.avg_y_max = None
        self.avg_x_min = None
        self.avg_x_max = None

        EVT_DATA_EVENT (self, self.format_data)

        self.input_watcher = input_watcher (info.msgq, self, info.frame_decim)

    def channel_color (self, ch):
        return self.channel_colors[ch % len(self.channel_colors)]
       
    def format_data (self, evt):
        if not self.info.running:
            return
        
        if self.info.xy:
            self.format_xy_data (evt)
            return

        info = self.info
        records = evt.data
        nchannels = len (records)
        npoints = len (records[0])

        objects = []

        Ts = 1.0 / (info.get_sample_rate () / info.get_decimation_rate ())
        x_vals = Ts * numpy.arange (-npoints/2, npoints/2)

        # preliminary clipping based on time axis here, instead of in graphics code
        time_per_window = self.info.get_time_per_div () * 10
        n = int (time_per_window / Ts + 0.5)
        n = n & ~0x1                    # make even
        n = max (2, min (n, npoints))

        self.SetXUseScopeTicks (True)   # use 10 divisions, no labels

        for ch in range(nchannels):
            r = records[ch]

            # plot middle n points of record

            lb = npoints/2 - n/2
            ub = npoints/2 + n/2
            # points = zip (x_vals[lb:ub], r[lb:ub])
            points = numpy.zeros ((ub-lb, 2), numpy.float64)
            points[:,0] = x_vals[lb:ub]
            points[:,1] = r[lb:ub]

            m = info.get_marker ()
            if m == 'line':
                objects.append (plot.PolyLine (points,
                                               colour=self.channel_color (ch),
                                               legend=('Ch%d' % (ch+1,))))
            else:
                objects.append (plot.PolyMarker (points,
                                                 marker=m,
                                                 colour=self.channel_color (ch),
                                                 legend=('Ch%d' % (ch+1,))))

        graphics = plot.PlotGraphics (objects,
                                      title=self.info.title,
                                      xLabel = '', yLabel = '')

        time_per_div = info.get_time_per_div ()
        x_range = (-5.0 * time_per_div, 5.0 * time_per_div) # ranges are tuples!
        volts_per_div = info.get_volts_per_div ()
        if not self.info.autorange:
            self.y_range = (-4.0 * volts_per_div, 4.0 * volts_per_div)
        self.Draw (graphics, xAxis=x_range, yAxis=self.y_range)
        self.update_y_range () # autorange to self.y_range


    def format_xy_data (self, evt):
        info = self.info
        records = evt.data
        nchannels = len (records)
        npoints = len (records[0])

        if nchannels < 2:
            return

        objects = []
        # points = zip (records[0], records[1])
        points = numpy.zeros ((len(records[0]), 2), numpy.float32)
        points[:,0] = records[0]
        points[:,1] = records[1]
        
        self.SetXUseScopeTicks (False)

        m = info.get_marker ()
        if m == 'line':
            objects.append (plot.PolyLine (points,
                                           colour=self.channel_color (0)))
        else:
            objects.append (plot.PolyMarker (points,
                                             marker=m,
                                             colour=self.channel_color (0)))

        graphics = plot.PlotGraphics (objects,
                                      title=self.info.title,
                                      xLabel = 'I', yLabel = 'Q')

        self.Draw (graphics, xAxis=self.x_range, yAxis=self.y_range)
        self.update_y_range ()
        self.update_x_range ()


    def update_y_range (self):
        alpha = 1.0/25
        graphics = self.last_draw[0]
        p1, p2 = graphics.boundingBox ()     # min, max points of graphics

        if self.avg_y_min: # prevent vertical scale from jumping abruptly --?
            self.avg_y_min = p1[1] * alpha + self.avg_y_min * (1 - alpha)
            self.avg_y_max = p2[1] * alpha + self.avg_y_max * (1 - alpha)
        else: # initial guess
            self.avg_y_min = p1[1] # -500.0 workaround, sometimes p1 is ~ 10^35
            self.avg_y_max = p2[1] # 500.0

        self.y_range = self._axisInterval ('auto', self.avg_y_min, self.avg_y_max)
        # print "p1 %s  p2 %s  y_min %s  y_max %s  y_range %s" \
        #        % (p1, p2, self.avg_y_min, self.avg_y_max, self.y_range)


    def update_x_range (self):
        alpha = 1.0/25
        graphics = self.last_draw[0]
        p1, p2 = graphics.boundingBox ()     # min, max points of graphics

        if self.avg_x_min:
            self.avg_x_min = p1[0] * alpha + self.avg_x_min * (1 - alpha)
            self.avg_x_max = p2[0] * alpha + self.avg_x_max * (1 - alpha)
        else:
            self.avg_x_min = p1[0]
            self.avg_x_max = p2[0]

        self.x_range = self._axisInterval ('auto', self.avg_x_min, self.avg_x_max)


# ----------------------------------------------------------------
# Stand-alone test application
# ----------------------------------------------------------------

class test_top_block (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        if len(argv) > 1:
            frame_decim = int(argv[1]) 
        else:
            frame_decim = 1

        if len(argv) > 2:
            v_scale = float(argv[2])  # start up at this v_scale value
        else:
            v_scale = None  # start up in autorange mode, default

        if len(argv) > 3:
            t_scale = float(argv[3])  # start up at this t_scale value
        else:
            t_scale = None  # old behavior

        print "frame decim %s  v_scale %s  t_scale %s" % (frame_decim,v_scale,t_scale)
            
        input_rate = 1e6

        # Generate a complex sinusoid
        self.src0 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 25.1e3, 1e3)

        # We add this throttle block so that this demo doesn't suck down
        # all the CPU available.  You normally wouldn't use it...
        self.thr = gr.throttle(gr.sizeof_gr_complex, input_rate)

        scope = scope_sink_c (panel,"Secret Data",sample_rate=input_rate,
                              frame_decim=frame_decim,
                              v_scale=v_scale, t_scale=t_scale)
        vbox.Add (scope.win, 1, wx.EXPAND)

        # Ultimately this will be
        # self.connect("src0 throttle scope")
	self.connect(self.src0, self.thr, scope) 

def main ():
    app = stdgui2.stdapp (test_top_block, "O'Scope Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()

# ----------------------------------------------------------------
