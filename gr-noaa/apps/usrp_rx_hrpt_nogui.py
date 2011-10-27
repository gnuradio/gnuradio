#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Thu Oct 27 13:49:01 2011
##################################################

from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from optparse import OptionParser
import ConfigParser
import math, os

class usrp_rx_hrpt_nogui(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "USRP HRPT Receiver")

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 4e6
		self.sps = sps = sample_rate/sym_rate
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(config_filename)
		try: pll_alpha = self._pll_alpha_config.getfloat('usrp_rx_hrpt', 'pll_alpha')
		except: pll_alpha = 0.01
		self.pll_alpha = pll_alpha
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(config_filename)
		try: output_filename = self._output_filename_config.get('usrp_rx_hrpt', 'filename')
		except: output_filename = 'frames.hrpt'
		self.output_filename = output_filename
		self.max_clock_offset = max_clock_offset = 100e-6
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(config_filename)
		try: gain = self._gain_config.getfloat('usrp_rx_hrpt', 'gain')
		except: gain = 35
		self.gain = gain
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(config_filename)
		try: freq = self._freq_config.getfloat('usrp_rx_hrpt', 'freq')
		except: freq = 1698e6
		self.freq = freq
		self._clock_alpha_config = ConfigParser.ConfigParser()
		self._clock_alpha_config.read(config_filename)
		try: clock_alpha = self._clock_alpha_config.getfloat('usrp_rx_hrpt', 'clock_alpha')
		except: clock_alpha = 0.01
		self.clock_alpha = clock_alpha
		self._addr_config = ConfigParser.ConfigParser()
		self._addr_config.read(config_filename)
		try: addr = self._addr_config.get('usrp_rx_hrpt', 'addr')
		except: addr = ""
		self.addr = addr

		##################################################
		# Blocks
		##################################################
		self.uhd_usrp_source_0 = uhd.usrp_source(
			device_addr=addr,
			io_type=uhd.io_type.COMPLEX_FLOAT32,
			num_channels=1,
		)
		self.uhd_usrp_source_0.set_samp_rate(sample_rate)
		self.uhd_usrp_source_0.set_center_freq(freq, 0)
		self.uhd_usrp_source_0.set_gain(0, 0)
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
		self.frame_sink = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.frame_sink.set_unbuffered(False)
		self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(sps/2.0, clock_alpha**2/4.0, 0.5, clock_alpha, max_clock_offset)
		self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
		self.deframer = noaa.hrpt_deframer()
		self.decoder = noaa.hrpt_decoder(True,True)
		self.agc = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_moving_average_xx_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))
		self.connect((self.pll, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.uhd_usrp_source_0, 0), (self.agc, 0))
		self.connect((self.agc, 0), (self.pll, 0))
		self.connect((self.digital_binary_slicer_fb_0, 0), (self.deframer, 0))
		self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))
		self.connect((self.deframer, 0), (self.decoder, 0))
		self.connect((self.deframer, 0), (self.frame_sink, 0))

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
		self.uhd_usrp_source_0.set_samp_rate(self.sample_rate)

	def get_sps(self):
		return self.sps

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))
		self.digital_clock_recovery_mm_xx_0.set_omega(self.sps/2.0)

	def get_config_filename(self):
		return self.config_filename

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._clock_alpha_config = ConfigParser.ConfigParser()
		self._clock_alpha_config.read(self.config_filename)
		if not self._clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._clock_alpha_config.add_section('usrp_rx_hrpt')
		self._clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._clock_alpha_config.write(open(self.config_filename, 'w'))
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(self.config_filename)
		if not self._pll_alpha_config.has_section('usrp_rx_hrpt'):
			self._pll_alpha_config.add_section('usrp_rx_hrpt')
		self._pll_alpha_config.set('usrp_rx_hrpt', 'pll_alpha', str(self.pll_alpha))
		self._pll_alpha_config.write(open(self.config_filename, 'w'))
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(self.config_filename)
		if not self._gain_config.has_section('usrp_rx_hrpt'):
			self._gain_config.add_section('usrp_rx_hrpt')
		self._gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._gain_config.write(open(self.config_filename, 'w'))
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(self.config_filename)
		if not self._freq_config.has_section('usrp_rx_hrpt'):
			self._freq_config.add_section('usrp_rx_hrpt')
		self._freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._freq_config.write(open(self.config_filename, 'w'))
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('usrp_rx_hrpt'):
			self._output_filename_config.add_section('usrp_rx_hrpt')
		self._output_filename_config.set('usrp_rx_hrpt', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))
		self._addr_config = ConfigParser.ConfigParser()
		self._addr_config.read(self.config_filename)
		if not self._addr_config.has_section('usrp_rx_hrpt'):
			self._addr_config.add_section('usrp_rx_hrpt')
		self._addr_config.set('usrp_rx_hrpt', 'addr', str(self.addr))
		self._addr_config.write(open(self.config_filename, 'w'))

	def get_pll_alpha(self):
		return self.pll_alpha

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(self.config_filename)
		if not self._pll_alpha_config.has_section('usrp_rx_hrpt'):
			self._pll_alpha_config.add_section('usrp_rx_hrpt')
		self._pll_alpha_config.set('usrp_rx_hrpt', 'pll_alpha', str(self.pll_alpha))
		self._pll_alpha_config.write(open(self.config_filename, 'w'))
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)

	def get_output_filename(self):
		return self.output_filename

	def set_output_filename(self, output_filename):
		self.output_filename = output_filename
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('usrp_rx_hrpt'):
			self._output_filename_config.add_section('usrp_rx_hrpt')
		self._output_filename_config.set('usrp_rx_hrpt', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))

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

	def get_gain(self):
		return self.gain

	def set_gain(self, gain):
		self.gain = gain
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(self.config_filename)
		if not self._gain_config.has_section('usrp_rx_hrpt'):
			self._gain_config.add_section('usrp_rx_hrpt')
		self._gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._gain_config.write(open(self.config_filename, 'w'))

	def get_freq(self):
		return self.freq

	def set_freq(self, freq):
		self.freq = freq
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(self.config_filename)
		if not self._freq_config.has_section('usrp_rx_hrpt'):
			self._freq_config.add_section('usrp_rx_hrpt')
		self._freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._freq_config.write(open(self.config_filename, 'w'))
		self.uhd_usrp_source_0.set_center_freq(self.freq, 0)

	def get_clock_alpha(self):
		return self.clock_alpha

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self._clock_alpha_config = ConfigParser.ConfigParser()
		self._clock_alpha_config.read(self.config_filename)
		if not self._clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._clock_alpha_config.add_section('usrp_rx_hrpt')
		self._clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._clock_alpha_config.write(open(self.config_filename, 'w'))
		self.digital_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.digital_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)

	def get_addr(self):
		return self.addr

	def set_addr(self, addr):
		self.addr = addr
		self._addr_config = ConfigParser.ConfigParser()
		self._addr_config.read(self.config_filename)
		if not self._addr_config.has_section('usrp_rx_hrpt'):
			self._addr_config.add_section('usrp_rx_hrpt')
		self._addr_config.set('usrp_rx_hrpt', 'addr', str(self.addr))
		self._addr_config.write(open(self.config_filename, 'w'))

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_hrpt_nogui()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()

