#
# Copyright 2008 Free Software Foundation, Inc.
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

##################################################
# Imports
##################################################
import scope_window
import common
from gnuradio import gr
from pubsub import pubsub
from constants import *

##################################################
# Scope sink block (wrapper for old wxgui)
##################################################
class _scope_sink_base(gr.hier_block2, common.prop_setter):
	"""
	A scope block with a gui window.
	"""

	def __init__(
		self,
		parent,
		title='',
		sample_rate=1,
		size=scope_window.DEFAULT_WIN_SIZE,
		frame_decim=None, #ignore (old wrapper)
		v_scale=scope_window.DEFAULT_V_SCALE,
		t_scale=None,
		num_inputs=1,
		ac_couple=False,
		xy_mode=False,
		frame_rate=scope_window.DEFAULT_FRAME_RATE,
	):
		if t_scale is None: t_scale = 0.001
		#init
		gr.hier_block2.__init__(
			self,
			"scope_sink",
			gr.io_signature(num_inputs, num_inputs, self._item_size),
			gr.io_signature(0, 0, 0),
		)
		#scope
		msgq = gr.msg_queue(2)
		scope = gr.oscope_sink_f(sample_rate, msgq)
		#connect
		if self._real:
			for i in range(num_inputs):
				self.connect((self, i), (scope, i))
		else:
			for i in range(num_inputs):
				c2f = gr.complex_to_float() 
				self.connect((self, i), c2f)
				self.connect((c2f, 0), (scope, 2*i+0))
				self.connect((c2f, 1), (scope, 2*i+1))
			num_inputs *= 2
		#controller
		self.controller = pubsub()
		self.controller.subscribe(SAMPLE_RATE_KEY, scope.set_sample_rate)
		self.controller.publish(SAMPLE_RATE_KEY, scope.sample_rate)
		def set_trigger_level(level):
			if level == '': scope.set_trigger_level_auto()
			else: scope.set_trigger_level(level)
		self.controller.subscribe(SCOPE_TRIGGER_LEVEL_KEY, set_trigger_level)
		def set_trigger_mode(mode):
			if mode == 0: mode = gr.gr_TRIG_AUTO
			elif mode < 0: mode = gr.gr_TRIG_NEG_SLOPE
			elif mode > 0: mode = gr.gr_TRIG_POS_SLOPE
			else: return
			scope.set_trigger_mode(mode)
		self.controller.subscribe(SCOPE_TRIGGER_MODE_KEY, set_trigger_mode)
		self.controller.subscribe(SCOPE_TRIGGER_CHANNEL_KEY, scope.set_trigger_channel)
		#start input watcher
		def setter(p, k, x): # lambdas can't have assignments :(
		    p[k] = x
		common.input_watcher(msgq, lambda x: setter(self.controller, MSG_KEY, x))
		#create window
		self.win = scope_window.scope_window(
			parent=parent,
			controller=self.controller,
			size=size,
			title=title,
			frame_rate=frame_rate,
			num_inputs=num_inputs,
			sample_rate_key=SAMPLE_RATE_KEY,
			t_scale=t_scale,
			v_scale=v_scale,
			ac_couple=ac_couple,
			xy_mode=xy_mode,
			scope_trigger_level_key=SCOPE_TRIGGER_LEVEL_KEY,
			scope_trigger_mode_key=SCOPE_TRIGGER_MODE_KEY,
			scope_trigger_channel_key=SCOPE_TRIGGER_CHANNEL_KEY,
			msg_key=MSG_KEY,
		)
		#register callbacks from window for external use
		for attr in filter(lambda a: a.startswith('set_'), dir(self.win)):
			setattr(self, attr, getattr(self.win, attr))
		self._register_set_prop(self.controller, SAMPLE_RATE_KEY)
		#backwards compadibility
		self.win.set_format_line = lambda: setter(self.win, MARKER_KEY, None)
		self.win.set_format_dot = lambda: setter(self.win, MARKER_KEY, 2.0)
		self.win.set_format_plus =  lambda: setter(self.win, MARKER_KEY, 3.0)

class scope_sink_f(_scope_sink_base):
	_item_size = gr.sizeof_float
	_real = True

class scope_sink_c(_scope_sink_base):
	_item_size = gr.sizeof_gr_complex
	_real = False

#backwards compadible wrapper (maybe only grc uses this)
class constellation_sink(scope_sink_c):
	def __init__(self, *args, **kwargs):
		scope_sink_c.__init__(self, *args, **kwargs)
		self.set_scope_xy_mode(True)

# ----------------------------------------------------------------
# Stand-alone test application
# ----------------------------------------------------------------

import wx
from gnuradio.wxgui import stdgui2

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
            t_scale = .00003  # old behavior

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
