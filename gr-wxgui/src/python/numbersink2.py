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
import number_window
import common
from gnuradio import gr, blks2
from pubsub import pubsub
from constants import *

##################################################
# Number sink block (wrapper for old wxgui)
##################################################
class _number_sink_base(gr.hier_block2, common.wxgui_hb):
	"""
	An decimator block with a number window display
	"""

	def __init__(
		self,
		parent,
		unit='units',
		minval=0,
		maxval=1,
		factor=1,
		decimal_places=3,
		ref_level=0,
		sample_rate=1,
		number_rate=number_window.DEFAULT_NUMBER_RATE,
		average=False,
		avg_alpha=None,
		label='Number Plot',
		size=number_window.DEFAULT_WIN_SIZE,
		peak_hold=False,
		show_gauge=True,
		**kwargs #catchall for backwards compatibility
	):
		#ensure avg alpha
		if avg_alpha is None: avg_alpha = 2.0/number_rate
		#init
		gr.hier_block2.__init__(
			self,
			"number_sink",
			gr.io_signature(1, 1, self._item_size),
			gr.io_signature(0, 0, 0),
		)
		#blocks
		sd = blks2.stream_to_vector_decimator(
			item_size=self._item_size,
			sample_rate=sample_rate,
			vec_rate=number_rate,
			vec_len=1,
		)
		if self._real:
			mult = gr.multiply_const_ff(factor)
			add = gr.add_const_ff(ref_level)
			avg = gr.single_pole_iir_filter_ff(1.0)
		else:
			mult = gr.multiply_const_cc(factor)
			add = gr.add_const_cc(ref_level)
			avg = gr.single_pole_iir_filter_cc(1.0)
		msgq = gr.msg_queue(2)
		sink = gr.message_sink(self._item_size, msgq, True)
		#controller
		self.controller = pubsub()
		self.controller.subscribe(SAMPLE_RATE_KEY, sd.set_sample_rate)
		self.controller.publish(SAMPLE_RATE_KEY, sd.sample_rate)
		self.controller[AVERAGE_KEY] = average
		self.controller[AVG_ALPHA_KEY] = avg_alpha
		def update_avg(*args):
			if self.controller[AVERAGE_KEY]: avg.set_taps(self.controller[AVG_ALPHA_KEY])
			else: avg.set_taps(1.0)
		update_avg()
		self.controller.subscribe(AVERAGE_KEY, update_avg)
		self.controller.subscribe(AVG_ALPHA_KEY, update_avg)
		#start input watcher
		common.input_watcher(msgq, self.controller, MSG_KEY)
		#create window
		self.win = number_window.number_window(
			parent=parent,
			controller=self.controller,
			size=size,
			title=label,
			units=unit,
			real=self._real,
			minval=minval,
			maxval=maxval,
			decimal_places=decimal_places,
			show_gauge=show_gauge,
			average_key=AVERAGE_KEY,
			avg_alpha_key=AVG_ALPHA_KEY,
			peak_hold=peak_hold,
			msg_key=MSG_KEY,
			sample_rate_key=SAMPLE_RATE_KEY,
		)
		common.register_access_methods(self, self.controller)
		#backwards compadibility
		self.set_show_gauge = self.win.show_gauges
		#connect
		self.wxgui_connect(self, sd, mult, add, avg, sink)

class number_sink_f(_number_sink_base):
	_item_size = gr.sizeof_float
	_real = True

class number_sink_c(_number_sink_base):
	_item_size = gr.sizeof_gr_complex
	_real = False

# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

import wx
from gnuradio.wxgui import stdgui2

class test_app_flow_graph (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        # build our flow graph
        input_rate = 20.48e3

        # Generate a real and complex sinusoids
        src1 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 2.21e3, 1)
        src2 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 2.21e3, 1)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        thr1 = gr.throttle(gr.sizeof_float, input_rate)
        thr2 = gr.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = number_sink_f (panel, unit='V',label="Real Data", avg_alpha=0.001,
                            sample_rate=input_rate, minval=-1, maxval=1,
                            ref_level=0, decimal_places=3)
        vbox.Add (sink1.win, 1, wx.EXPAND)
        sink2 = number_sink_c (panel, unit='V',label="Complex Data", avg_alpha=0.001,
                            sample_rate=input_rate, minval=-1, maxval=1,
                            ref_level=0, decimal_places=3)
        vbox.Add (sink2.win, 1, wx.EXPAND)

        self.connect (src1, thr1, sink1)
        self.connect (src2, thr2, sink2)

def main ():
    app = stdgui2.stdapp (test_app_flow_graph, "Number Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()

