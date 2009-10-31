#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Demod Hrpt File
# Generated: Sun Nov  1 18:39:07 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import math, os

class demod_hrpt_file(gr.top_block):

	def __init__(self, decim=16, pll_alpha=0.05, clock_alpha=0.05, sync_alpha=0.05, input_filename="usrp.dat", output_filename="frames.dat"):
		gr.top_block.__init__(self, "Demod Hrpt File")

		##################################################
		# Parameters
		##################################################
		self.decim = decim
		self.pll_alpha = pll_alpha
		self.clock_alpha = clock_alpha
		self.sync_alpha = sync_alpha
		self.input_filename = input_filename
		self.output_filename = output_filename

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self.max_clock_offset = max_clock_offset = 0.1
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)

		##################################################
		# Blocks
		##################################################
		self.agc = gr.agc_cc(1e-5, 1.0, 1.0/32768.0, 1.0)
		self.cs2cf = gr.interleaved_short_to_complex()
		self.file_source = gr.file_source(gr.sizeof_short*1, input_filename, False)
		self.gr_binary_slicer_fb_0 = gr.binary_slicer_fb()
		self.gr_clock_recovery_mm_xx_0 = gr.clock_recovery_mm_ff(sps/2.0, clock_alpha**2/4.0, 0.5, clock_alpha, max_clock_offset)
		self.gr_file_sink_0 = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
		self.noaa_hrpt_bit_sync_0 = noaa.hrpt_bit_sync()
		self.noaa_hrpt_decoder_0 = noaa.hrpt_decoder()
		self.noaa_hrpt_deframer_0 = noaa.hrpt_deframer()
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)

		##################################################
		# Connections
		##################################################
		self.connect((self.file_source, 0), (self.cs2cf, 0))
		self.connect((self.gr_clock_recovery_mm_xx_0, 0), (self.gr_binary_slicer_fb_0, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.gr_clock_recovery_mm_xx_0, 0))
		self.connect((self.pll, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.gr_binary_slicer_fb_0, 0), (self.noaa_hrpt_bit_sync_0, 0))
		self.connect((self.cs2cf, 0), (self.agc, 0))
		self.connect((self.agc, 0), (self.pll, 0))
		self.connect((self.noaa_hrpt_deframer_0, 0), (self.gr_file_sink_0, 0))
		self.connect((self.noaa_hrpt_deframer_0, 0), (self.noaa_hrpt_decoder_0, 0))
		self.connect((self.noaa_hrpt_bit_sync_0, 0), (self.noaa_hrpt_deframer_0, 0))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self.gr_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.gr_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)

	def set_sync_alpha(self, sync_alpha):
		self.sync_alpha = sync_alpha

	def set_input_filename(self, input_filename):
		self.input_filename = input_filename

	def set_output_filename(self, output_filename):
		self.output_filename = output_filename

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
		self.gr_clock_recovery_mm_xx_0.set_omega(self.sps/2.0)

	def set_max_clock_offset(self, max_clock_offset):
		self.max_clock_offset = max_clock_offset

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.pll.set_max_offset(self.max_carrier_offset)

	def set_hs(self, hs):
		self.hs = hs
		self.gr_moving_average_xx_0.set_length_and_scale(self.hs, 1.0/self.hs)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=16,
		help="Set decim [default=%default]")
	parser.add_option("-p", "--pll-alpha", dest="pll_alpha", type="eng_float", default=eng_notation.num_to_str(0.05),
		help="Set pll_alpha [default=%default]")
	parser.add_option("-a", "--clock-alpha", dest="clock_alpha", type="eng_float", default=eng_notation.num_to_str(0.05),
		help="Set clock_alpha [default=%default]")
	parser.add_option("-s", "--sync-alpha", dest="sync_alpha", type="eng_float", default=eng_notation.num_to_str(0.05),
		help="Set sync_alpha [default=%default]")
	parser.add_option("-F", "--input-filename", dest="input_filename", type="string", default="usrp.dat",
		help="Set usrp.dat [default=%default]")
	parser.add_option("-o", "--output-filename", dest="output_filename", type="string", default="frames.dat",
		help="Set frames.dat [default=%default]")
	(options, args) = parser.parse_args()
	tb = demod_hrpt_file(decim=options.decim, pll_alpha=options.pll_alpha, clock_alpha=options.clock_alpha, sync_alpha=options.sync_alpha, input_filename=options.input_filename, output_filename=options.output_filename)
	tb.run()

