#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Howto Square
# Generated: Thu Nov 12 11:26:07 2009
##################################################

import howto
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import wx

class howto_square(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="Howto Square")

		##################################################
		# Variables
		##################################################
		self.samp_rate = samp_rate = 10e3

		##################################################
		# Blocks
		##################################################
		self.sink = scopesink2.scope_sink_f(
			self.GetWin(),
			title="Input",
			sample_rate=samp_rate,
			v_scale=20,
			v_offset=0,
			t_scale=0.002,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.Add(self.sink.win)
		self.sink2 = scopesink2.scope_sink_f(
			self.GetWin(),
			title="Output",
			sample_rate=samp_rate,
			v_scale=0,
			v_offset=0,
			t_scale=0.002,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.Add(self.sink2.win)
		self.sqr = howto.square_ff()
		self.src = gr.vector_source_f(([float(n)-50 for n in range(100)]), True, 1)
		self.thr = gr.throttle(gr.sizeof_float*1, samp_rate)

		##################################################
		# Connections
		##################################################
		self.connect((self.thr, 0), (self.sqr, 0))
		self.connect((self.src, 0), (self.thr, 0))
		self.connect((self.thr, 0), (self.sink, 0))
		self.connect((self.sqr, 0), (self.sink2, 0))

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.sink.set_sample_rate(self.samp_rate)
		self.sink2.set_sample_rate(self.samp_rate)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = howto_square()
	tb.Run(True)

