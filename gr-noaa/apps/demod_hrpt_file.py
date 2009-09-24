#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Wed Sep 23 11:37:25 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import math

class demod_hrpt_file(gr.top_block):

	def __init__(self, decim=16, pll_alpha=0.05, sync_alpha=0.05, filename="usrp.dat", output="frames.dat"):
		gr.top_block.__init__(self, "USRP HRPT Receiver")

		##################################################
		# Parameters
		##################################################
		self.decim = decim
		self.pll_alpha = pll_alpha
		self.sync_alpha = sync_alpha
		self.filename = filename
		self.output = output

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self.max_sync_offset = max_sync_offset = 0.01
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)

		##################################################
		# Blocks
		##################################################
		self.decoder = noaa.hrpt_decoder()
		self.deframer = noaa.hrpt_deframer()
		self.deinterleave = gr.deinterleave(gr.sizeof_float*1)
		self.f2c = gr.float_to_complex(1)
		self.file_sink = gr.file_sink(gr.sizeof_short*1, output)
		self.file_source = gr.file_source(gr.sizeof_short*1, filename, False)
		self.matched_filter = gr.moving_average_cc(hs, 1.0/hs, 4000)
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.s2f = gr.short_to_float()
		self.sync = noaa.hrpt_sync_fb(sync_alpha, sync_alpha**2/4.0, sps, max_sync_offset)

		##################################################
		# Connections
		##################################################
		self.connect((self.deframer, 0), (self.file_sink, 0))
		self.connect((self.sync, 0), (self.deframer, 0))
		self.connect((self.pll, 0), (self.sync, 0))
		self.connect((self.matched_filter, 0), (self.pll, 0))
		self.connect((self.s2f, 0), (self.deinterleave, 0))
		self.connect((self.deinterleave, 1), (self.f2c, 1))
		self.connect((self.deinterleave, 0), (self.f2c, 0))
		self.connect((self.file_source, 0), (self.s2f, 0))
		self.connect((self.deframer, 0), (self.decoder, 0))
		self.connect((self.f2c, 0), (self.matched_filter, 0))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)

	def set_sync_alpha(self, sync_alpha):
		self.sync_alpha = sync_alpha
		self.sync.set_alpha(self.sync_alpha)
		self.sync.set_beta(self.sync_alpha**2/4.0)

	def set_filename(self, filename):
		self.filename = filename

	def set_output(self, output):
		self.output = output

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

	def set_max_sync_offset(self, max_sync_offset):
		self.max_sync_offset = max_sync_offset
		self.sync.set_max_offset(self.max_sync_offset)

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.pll.set_max_offset(self.max_carrier_offset)

	def set_hs(self, hs):
		self.hs = hs
		self.matched_filter.set_length_and_scale(self.hs, 1.0/self.hs)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=16,
		help="Set Decimation [default=%default]")
	parser.add_option("-p", "--pll-alpha", dest="pll_alpha", type="eng_float", default=eng_notation.num_to_str(0.05),
		help="Set pll_alpha [default=%default]")
	parser.add_option("-s", "--sync-alpha", dest="sync_alpha", type="eng_float", default=eng_notation.num_to_str(0.05),
		help="Set sync_alpha [default=%default]")
	parser.add_option("-F", "--filename", dest="filename", type="string", default="usrp.dat",
		help="Set Filename [default=%default]")
	parser.add_option("-o", "--output", dest="output", type="string", default="frames.dat",
		help="Set Output [default=%default]")
	(options, args) = parser.parse_args()
	tb = demod_hrpt_file(decim=options.decim, pll_alpha=options.pll_alpha, sync_alpha=options.sync_alpha, filename=options.filename, output=options.output)
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()

