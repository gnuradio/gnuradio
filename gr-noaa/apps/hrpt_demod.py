#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Hrpt Demod
# Generated: Thu Oct 27 13:51:59 2011
##################################################

from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import math, os

class hrpt_demod(gr.top_block):

	def __init__(self, input_filename="usrp.dat", output_filename="frames.dat", decim=32, pll_alpha=0.01, clock_alpha=0.01):
		gr.top_block.__init__(self, "Hrpt Demod")

		##################################################
		# Parameters
		##################################################
		self.input_filename = input_filename
		self.output_filename = output_filename
		self.decim = decim
		self.pll_alpha = pll_alpha
		self.clock_alpha = clock_alpha

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 4e6
		self.sps = sps = sample_rate/sym_rate
		self.max_clock_offset = max_clock_offset = 100e-6
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)

		##################################################
		# Blocks
		##################################################
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.noaa_hrpt_deframer_0 = noaa.hrpt_deframer()
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
		self.gr_file_sink_0 = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.gr_file_sink_0.set_unbuffered(False)
		self.file_source = gr.file_source(gr.sizeof_short*1, input_filename, False)
		self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(sps/2.0, clock_alpha**2/4.0, 0.5, clock_alpha, max_clock_offset)
		self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
		self.decoder = noaa.hrpt_decoder(True,False)
		self.cs2cf = gr.interleaved_short_to_complex()
		self.agc = gr.agc_cc(1e-5, 1.0, 1.0/32768.0, 1.0)

		##################################################
		# Connections
		##################################################
		self.connect((self.file_source, 0), (self.cs2cf, 0))
		self.connect((self.pll, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.cs2cf, 0), (self.agc, 0))
		self.connect((self.agc, 0), (self.pll, 0))
		self.connect((self.noaa_hrpt_deframer_0, 0), (self.gr_file_sink_0, 0))
		self.connect((self.noaa_hrpt_deframer_0, 0), (self.decoder, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))
		self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))
		self.connect((self.digital_binary_slicer_fb_0, 0), (self.noaa_hrpt_deframer_0, 0))

	def get_input_filename(self):
		return self.input_filename

	def set_input_filename(self, input_filename):
		self.input_filename = input_filename

	def get_output_filename(self):
		return self.output_filename

	def set_output_filename(self, output_filename):
		self.output_filename = output_filename

	def get_decim(self):
		return self.decim

	def set_decim(self, decim):
		self.decim = decim

	def get_pll_alpha(self):
		return self.pll_alpha

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)

	def get_clock_alpha(self):
		return self.clock_alpha

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self.digital_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.digital_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)

	def get_sym_rate(self):
		return self.sym_rate

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)

	def get_sample_rate(self):
		return self.sample_rate

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.set_sps(self.sample_rate/self.sym_rate)

	def get_sps(self):
		return self.sps

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))
		self.digital_clock_recovery_mm_xx_0.set_omega(self.sps/2.0)

	def get_max_clock_offset(self):
		return self.max_clock_offset

	def set_max_clock_offset(self, max_clock_offset):
		self.max_clock_offset = max_clock_offset

	def get_max_carrier_offset(self):
		return self.max_carrier_offset

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.pll.set_max_offset(self.max_carrier_offset)

	def get_hs(self):
		return self.hs

	def set_hs(self, hs):
		self.hs = hs
		self.gr_moving_average_xx_0.set_length_and_scale(self.hs, 1.0/self.hs)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-F", "--input-filename", dest="input_filename", type="string", default="usrp.dat",
		help="Set usrp.dat [default=%default]")
	parser.add_option("-o", "--output-filename", dest="output_filename", type="string", default="frames.dat",
		help="Set frames.dat [default=%default]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=32,
		help="Set None [default=%default]")
	parser.add_option("-p", "--pll-alpha", dest="pll_alpha", type="eng_float", default=eng_notation.num_to_str(0.01),
		help="Set None [default=%default]")
	parser.add_option("-s", "--clock-alpha", dest="clock_alpha", type="eng_float", default=eng_notation.num_to_str(0.01),
		help="Set None [default=%default]")
	(options, args) = parser.parse_args()
	tb = hrpt_demod(input_filename=options.input_filename, output_filename=options.output_filename, decim=options.decim, pll_alpha=options.pll_alpha, clock_alpha=options.clock_alpha)
	tb.run()

