#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Hrpt Decode
# Generated: Sun Nov  8 10:49:01 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import math, os

class hrpt_decode(gr.top_block):

	def __init__(self, decim=32, pll_alpha=0.01, clock_alpha=0.01, input_filename="frames.hrpt"):
		gr.top_block.__init__(self, "Hrpt Decode")

		##################################################
		# Parameters
		##################################################
		self.decim = decim
		self.pll_alpha = pll_alpha
		self.clock_alpha = clock_alpha
		self.input_filename = input_filename

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self.max_clock_offset = max_clock_offset = 100e-6
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)

		##################################################
		# Blocks
		##################################################
		self.decoder = noaa.hrpt_decoder(True,True)
		self.file_source = gr.file_source(gr.sizeof_short*1, input_filename, False)

		##################################################
		# Connections
		##################################################
		self.connect((self.file_source, 0), (self.decoder, 0))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha

	def set_input_filename(self, input_filename):
		self.input_filename = input_filename

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.set_sps(self.sample_rate/self.sym_rate)

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))

	def set_max_clock_offset(self, max_clock_offset):
		self.max_clock_offset = max_clock_offset

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset

	def set_hs(self, hs):
		self.hs = hs

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=32,
		help="Set decim [default=%default]")
	parser.add_option("-p", "--pll-alpha", dest="pll_alpha", type="eng_float", default=eng_notation.num_to_str(0.01),
		help="Set pll_alpha [default=%default]")
	parser.add_option("-s", "--clock-alpha", dest="clock_alpha", type="eng_float", default=eng_notation.num_to_str(0.01),
		help="Set clock_alpha [default=%default]")
	parser.add_option("-F", "--input-filename", dest="input_filename", type="string", default="frames.hrpt",
		help="Set frames.hrpt [default=%default]")
	(options, args) = parser.parse_args()
	tb = hrpt_decode(decim=options.decim, pll_alpha=options.pll_alpha, clock_alpha=options.clock_alpha, input_filename=options.input_filename)
	tb.run()

