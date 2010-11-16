#
# Copyright 2008,2010 Free Software Foundation, Inc.
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
import math

class ac_couple_block(gr.hier_block2):
	"""
	AC couple the incoming stream by subtracting out the low pass signal.
	Mute the low pass filter to disable ac coupling.
	"""

	def __init__(self, controller, ac_couple_key, sample_rate_key):
		gr.hier_block2.__init__(
			self,
			"ac_couple",
			gr.io_signature(1, 1, gr.sizeof_float),
			gr.io_signature(1, 1, gr.sizeof_float),
		)
		#blocks
		lpf = gr.single_pole_iir_filter_ff(0.0)
		sub = gr.sub_ff()
		mute = gr.mute_ff()
		#connect
		self.connect(self, sub, self)
		self.connect(self, lpf, mute, (sub, 1))
		#subscribe
		controller.subscribe(ac_couple_key, lambda x: mute.set_mute(not x))
		controller.subscribe(sample_rate_key, lambda x: lpf.set_taps(0.05))
		#initialize
		controller[ac_couple_key] = controller[ac_couple_key]
		controller[sample_rate_key] = controller[sample_rate_key]

##################################################
# Scope sink block (wrapper for old wxgui)
##################################################
class _scope_sink_base(gr.hier_block2, common.wxgui_hb):
	"""
	A scope block with a gui window.
	"""

	def __init__(
		self,
		parent,
		title='',
		sample_rate=1,
		size=scope_window.DEFAULT_WIN_SIZE,
		v_scale=0,
		t_scale=0,
		v_offset=0,
		xy_mode=False,
		ac_couple=False,
		num_inputs=1,
		trig_mode=scope_window.DEFAULT_TRIG_MODE,
		y_axis_label='Counts',
		frame_rate=scope_window.DEFAULT_FRAME_RATE,
                use_persistence=False,
                persist_alpha=None,
		**kwargs #do not end with a comma
	):
                #ensure analog alpha
                if persist_alpha is None: 
                  actual_frame_rate=float(frame_rate)
                  analog_cutoff_freq=0.5 # Hertz
                  #calculate alpha from wanted cutoff freq
                  persist_alpha = 1.0 - math.exp(-2.0*math.pi*analog_cutoff_freq/actual_frame_rate)

		if not t_scale: t_scale = 10.0/sample_rate
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
		#controller
		self.controller = pubsub()
		self.controller.subscribe(SAMPLE_RATE_KEY, scope.set_sample_rate)
		self.controller.publish(SAMPLE_RATE_KEY, scope.sample_rate)
		self.controller.subscribe(DECIMATION_KEY, scope.set_decimation_count)
		self.controller.publish(DECIMATION_KEY, scope.get_decimation_count)
		self.controller.subscribe(TRIGGER_LEVEL_KEY, scope.set_trigger_level)
		self.controller.publish(TRIGGER_LEVEL_KEY, scope.get_trigger_level)
		self.controller.subscribe(TRIGGER_MODE_KEY, scope.set_trigger_mode)
		self.controller.publish(TRIGGER_MODE_KEY, scope.get_trigger_mode)
		self.controller.subscribe(TRIGGER_SLOPE_KEY, scope.set_trigger_slope)
		self.controller.publish(TRIGGER_SLOPE_KEY, scope.get_trigger_slope)
		self.controller.subscribe(TRIGGER_CHANNEL_KEY, scope.set_trigger_channel)
		self.controller.publish(TRIGGER_CHANNEL_KEY, scope.get_trigger_channel)
		actual_num_inputs = self._real and num_inputs or num_inputs*2
		#init ac couple
		for i in range(actual_num_inputs):
			self.controller[common.index_key(AC_COUPLE_KEY, i)] = ac_couple
		#start input watcher
		common.input_watcher(msgq, self.controller, MSG_KEY)
		#create window
		self.win = scope_window.scope_window(
			parent=parent,
			controller=self.controller,
			size=size,
			title=title,
			frame_rate=frame_rate,
			num_inputs=actual_num_inputs,
			sample_rate_key=SAMPLE_RATE_KEY,
			t_scale=t_scale,
			v_scale=v_scale,
			v_offset=v_offset,
			xy_mode=xy_mode,
			trig_mode=trig_mode,
			y_axis_label=y_axis_label,
			ac_couple_key=AC_COUPLE_KEY,
			trigger_level_key=TRIGGER_LEVEL_KEY,
			trigger_mode_key=TRIGGER_MODE_KEY,
			trigger_slope_key=TRIGGER_SLOPE_KEY,
			trigger_channel_key=TRIGGER_CHANNEL_KEY,
			decimation_key=DECIMATION_KEY,
			msg_key=MSG_KEY,
                        use_persistence=use_persistence,
                        persist_alpha=persist_alpha,
		)
		common.register_access_methods(self, self.win)
		#connect
		if self._real:
			for i in range(num_inputs):
				self.wxgui_connect(
					(self, i),
					ac_couple_block(self.controller, common.index_key(AC_COUPLE_KEY, i), SAMPLE_RATE_KEY),
					(scope, i),
				)
		else:
			for i in range(num_inputs):
				c2f = gr.complex_to_float() 
				self.wxgui_connect((self, i), c2f)
				for j in range(2):
					self.connect(
						(c2f, j), 
						ac_couple_block(self.controller, common.index_key(AC_COUPLE_KEY, 2*i+j), SAMPLE_RATE_KEY),
						(scope, 2*i+j),
					)

class scope_sink_f(_scope_sink_base):
	_item_size = gr.sizeof_float
	_real = True

class scope_sink_c(_scope_sink_base):
	_item_size = gr.sizeof_gr_complex
	_real = False

# ----------------------------------------------------------------
# Stand-alone test application
# ----------------------------------------------------------------

import wx
from gnuradio.wxgui import stdgui2

class test_top_block (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        default_input_rate = 1e6
        if len(argv) > 1:
            input_rate = int(argv[1]) 
        else:
            input_rate = default_input_rate

        if len(argv) > 2:
            v_scale = float(argv[2])  # start up at this v_scale value
        else:
            v_scale = None  # start up in autorange mode, default

        if len(argv) > 3:
            t_scale = float(argv[3])  # start up at this t_scale value
        else:
            t_scale = .00003*default_input_rate/input_rate # old behavior

        print "input rate %s  v_scale %s  t_scale %s" % (input_rate,v_scale,t_scale)
            

        # Generate a complex sinusoid
        ampl=1.0e3
        self.src0 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 25.1e3*input_rate/default_input_rate, ampl)
        self.noise =gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 11.1*25.1e3*input_rate/default_input_rate, ampl/10) 
        #self.noise =gr.noise_source_c(gr.GR_GAUSSIAN, ampl/10)
        self.combine=gr.add_cc()

        # We add this throttle block so that this demo doesn't suck down
        # all the CPU available.  You normally wouldn't use it...
        self.thr = gr.throttle(gr.sizeof_gr_complex, input_rate)

        scope = scope_sink_c (panel,"Secret Data",sample_rate=input_rate,
                              v_scale=v_scale, t_scale=t_scale)
        vbox.Add (scope.win, 1, wx.EXPAND)

        # Ultimately this will be
        # self.connect("src0 throttle scope")
	self.connect(self.src0,(self.combine,0))
        self.connect(self.noise,(self.combine,1))
        self.connect(self.combine, self.thr, scope) 

def main ():
    app = stdgui2.stdapp (test_top_block, "O'Scope Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
