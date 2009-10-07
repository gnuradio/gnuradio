#
# Copyright 2009 Free Software Foundation, Inc.
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
import histo_window
import common
from gnuradio import gr, blks2
from pubsub import pubsub
from constants import *

##################################################
# histo sink block (wrapper for old wxgui)
##################################################
class histo_sink_f(gr.hier_block2, common.wxgui_hb):
	"""
	A histogram block and a gui window.
	"""

	def __init__(
		self,
		parent,
		size=histo_window.DEFAULT_WIN_SIZE,
		title='',
		num_bins=11,
		frame_size=1000,
	):
		#init
		gr.hier_block2.__init__(
			self,
			"histo_sink",
			gr.io_signature(1, 1, gr.sizeof_float),
			gr.io_signature(0, 0, 0),
		)
		#blocks
		msgq = gr.msg_queue(2)
		histo = gr.histo_sink_f(msgq)
		histo.set_num_bins(num_bins)
		histo.set_frame_size(frame_size)
		#controller
		self.controller = pubsub()
		self.controller.subscribe(NUM_BINS_KEY, histo.set_num_bins)
		self.controller.publish(NUM_BINS_KEY, histo.get_num_bins)
		self.controller.subscribe(FRAME_SIZE_KEY, histo.set_frame_size)
		self.controller.publish(FRAME_SIZE_KEY, histo.get_frame_size)
		#start input watcher
		common.input_watcher(msgq, self.controller, MSG_KEY, arg1_key=MINIMUM_KEY, arg2_key=MAXIMUM_KEY)
		#create window
		self.win = histo_window.histo_window(
			parent=parent,
			controller=self.controller,
			size=size,
			title=title,
			maximum_key=MAXIMUM_KEY,
			minimum_key=MINIMUM_KEY,
			num_bins_key=NUM_BINS_KEY,
			frame_size_key=FRAME_SIZE_KEY,
			msg_key=MSG_KEY,
		)
		common.register_access_methods(self, self.win)
		#connect
		self.wxgui_connect(self, histo)

# ----------------------------------------------------------------
# Standalone test app
# ----------------------------------------------------------------

import wx
from gnuradio.wxgui import stdgui2

class test_app_block (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        # build our flow graph
        input_rate = 20.48e3

        src2 = gr.sig_source_f (input_rate, gr.GR_SIN_WAVE, 2e3, 1)
        #src2 = gr.sig_source_f (input_rate, gr.GR_CONST_WAVE, 5.75e3, 1)
        thr2 = gr.throttle(gr.sizeof_float, input_rate)
        sink2 = histo_sink_f (panel, title="Data", num_bins=31, frame_size=1000)
        vbox.Add (sink2.win, 1, wx.EXPAND)

        self.connect(src2, thr2, sink2)

def main ():
    app = stdgui2.stdapp (test_app_block, "Histo Sink Test App")
    app.MainLoop ()

if __name__ == '__main__':
    main ()
