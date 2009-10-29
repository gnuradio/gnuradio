#
# Copyright 2008,2009 Free Software Foundation, Inc.
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
import waterfall_window
import common
from gnuradio import gr, blks2
from pubsub import pubsub
from constants import *

##################################################
# Waterfall sink block (wrapper for old wxgui)
##################################################
class _waterfall_sink_base(gr.hier_block2, common.wxgui_hb):
	"""
	An fft block with real/complex inputs and a gui window.
	"""

	def __init__(
		self,
		parent,
		baseband_freq=0,
		ref_level=50,
		sample_rate=1,
		fft_size=512,
		fft_rate=waterfall_window.DEFAULT_FRAME_RATE,
		average=False,
		avg_alpha=None,
		title='',
		size=waterfall_window.DEFAULT_WIN_SIZE,
		ref_scale=2.0,
		dynamic_range=80,
		num_lines=256,
		win=None,
		**kwargs #do not end with a comma
	):
		#ensure avg alpha
		if avg_alpha is None: avg_alpha = 2.0/fft_rate
		#init
		gr.hier_block2.__init__(
			self,
			"waterfall_sink",
			gr.io_signature(1, 1, self._item_size),
			gr.io_signature(0, 0, 0),
		)
		#blocks
		fft = self._fft_chain(
			sample_rate=sample_rate,
			fft_size=fft_size,
			frame_rate=fft_rate,
			ref_scale=ref_scale,
			avg_alpha=avg_alpha,
			average=average,
			win=win,
		)
		msgq = gr.msg_queue(2)
		sink = gr.message_sink(gr.sizeof_float*fft_size, msgq, True)
		#controller
		self.controller = pubsub()
		self.controller.subscribe(AVERAGE_KEY, fft.set_average)
		self.controller.publish(AVERAGE_KEY, fft.average)
		self.controller.subscribe(AVG_ALPHA_KEY, fft.set_avg_alpha)
		self.controller.publish(AVG_ALPHA_KEY, fft.avg_alpha)
		self.controller.subscribe(SAMPLE_RATE_KEY, fft.set_sample_rate)
		self.controller.publish(SAMPLE_RATE_KEY, fft.sample_rate)
		self.controller.subscribe(DECIMATION_KEY, fft.set_decimation)
		self.controller.publish(DECIMATION_KEY, fft.decimation)
		self.controller.subscribe(FRAME_RATE_KEY, fft.set_vec_rate)
		self.controller.publish(FRAME_RATE_KEY, fft.frame_rate)
		#start input watcher
		common.input_watcher(msgq, self.controller, MSG_KEY)
		#create window
		self.win = waterfall_window.waterfall_window(
			parent=parent,
			controller=self.controller,
			size=size,
			title=title,
			real=self._real,
			fft_size=fft_size,
			num_lines=num_lines,
			baseband_freq=baseband_freq,
			decimation_key=DECIMATION_KEY,
			sample_rate_key=SAMPLE_RATE_KEY,
			frame_rate_key=FRAME_RATE_KEY,
			dynamic_range=dynamic_range,
			ref_level=ref_level,
			average_key=AVERAGE_KEY,
			avg_alpha_key=AVG_ALPHA_KEY,
			msg_key=MSG_KEY,
		)
		common.register_access_methods(self, self.win)
		setattr(self.win, 'set_baseband_freq', getattr(self, 'set_baseband_freq')) #BACKWARDS
		#connect
		self.wxgui_connect(self, fft, sink)

class waterfall_sink_f(_waterfall_sink_base):
	_fft_chain = blks2.logpwrfft_f
	_item_size = gr.sizeof_float
	_real = True

class waterfall_sink_c(_waterfall_sink_base):
	_fft_chain = blks2.logpwrfft_c
	_item_size = gr.sizeof_gr_complex
	_real = False

# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

import wx
from gnuradio.wxgui import stdgui2

class test_top_block (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        fft_size = 512

        # build our flow graph
        input_rate = 20.000e3

        # Generate a complex sinusoid
        self.src1 = gr.sig_source_c (input_rate, gr.GR_SIN_WAVE, 5.75e3, 1000)
        #src1 = gr.sig_source_c (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1000)

        # We add these throttle blocks so that this demo doesn't
        # suck down all the CPU available.  Normally you wouldn't use these.
        self.thr1 = gr.throttle(gr.sizeof_gr_complex, input_rate)

        sink1 = waterfall_sink_c (panel, title="Complex Data", fft_size=fft_size,
                                  sample_rate=input_rate, baseband_freq=100e3)
	self.connect(self.src1, self.thr1, sink1)
        vbox.Add (sink1.win, 1, wx.EXPAND)

        # generate a real sinusoid
        self.src2 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 5.75e3, 1000)
        self.thr2 = gr.throttle(gr.sizeof_float, input_rate)
        sink2 = waterfall_sink_f (panel, title="Real Data", fft_size=fft_size,
                                  sample_rate=input_rate, baseband_freq=100e3)
	self.connect(self.src2, self.thr2, sink2)
        vbox.Add (sink2.win, 1, wx.EXPAND)


def main ():
    app = stdgui2.stdapp (test_top_block, "Waterfall Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()

