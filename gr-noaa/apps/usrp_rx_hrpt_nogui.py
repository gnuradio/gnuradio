#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Usrp Rx Hrpt Nogui
# Generated: Sun Nov  1 19:32:32 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from grc_gnuradio import usrp as grc_usrp
from optparse import OptionParser
import ConfigParser
import math, os

class usrp_rx_hrpt_nogui(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "Usrp Rx Hrpt Nogui")

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(config_filename)
		try: decim = self._decim_config.getfloat('usrp_rx_hrpt', 'decim')
		except: decim = 16
		self.decim = decim
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(config_filename)
		try: side = self._side_config.get('usrp_rx_hrpt', 'side')
		except: side = 'A'
		self.side = side
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(config_filename)
		try: pll_alpha = self._pll_alpha_config.getfloat('usrp_rx_hrpt', 'pll_alpha')
		except: pll_alpha = 0.05
		self.pll_alpha = pll_alpha
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(config_filename)
		try: output_filename = self._output_filename_config.get('usrp_rx_hrpt', 'filename')
		except: output_filename = 'frames.dat'
		self.output_filename = output_filename
		self.max_clock_offset = max_clock_offset = 0.1
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
		except: clock_alpha = 0.05
		self.clock_alpha = clock_alpha

		##################################################
		# Blocks
		##################################################
		self.agc = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)
		self.decoder = noaa.hrpt_decoder()
		self.deframer = noaa.hrpt_deframer()
		self.frame_sink = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.gr_binary_slicer_fb_0 = gr.binary_slicer_fb()
		self.gr_clock_recovery_mm_xx_0 = gr.clock_recovery_mm_ff(sps/2.0, clock_alpha**2/4.0, 0.5, clock_alpha, max_clock_offset)
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
		self.noaa_hrpt_bit_sync_0 = noaa.hrpt_bit_sync()
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.usrp_source = grc_usrp.simple_source_c(which=0, side=side, rx_ant="RXA")
		self.usrp_source.set_decim_rate(decim)
		self.usrp_source.set_frequency(freq, verbose=True)
		self.usrp_source.set_gain(gain)

		##################################################
		# Connections
		##################################################
		self.connect((self.usrp_source, 0), (self.agc, 0))
		self.connect((self.agc, 0), (self.pll, 0))
		self.connect((self.pll, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.gr_clock_recovery_mm_xx_0, 0))
		self.connect((self.noaa_hrpt_bit_sync_0, 0), (self.deframer, 0))
		self.connect((self.gr_binary_slicer_fb_0, 0), (self.noaa_hrpt_bit_sync_0, 0))
		self.connect((self.gr_clock_recovery_mm_xx_0, 0), (self.gr_binary_slicer_fb_0, 0))
		self.connect((self.deframer, 0), (self.frame_sink, 0))
		self.connect((self.deframer, 0), (self.decoder, 0))

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._clock_alpha_config = ConfigParser.ConfigParser()
		self._clock_alpha_config.read(self.config_filename)
		if not self._clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._clock_alpha_config.add_section('usrp_rx_hrpt')
		self._clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._clock_alpha_config.write(open(self.config_filename, 'w'))
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
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(self.config_filename)
		if not self._decim_config.has_section('usrp_rx_hrpt'):
			self._decim_config.add_section('usrp_rx_hrpt')
		self._decim_config.set('usrp_rx_hrpt', 'decim', str(self.decim))
		self._decim_config.write(open(self.config_filename, 'w'))
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('usrp_rx_hrpt'):
			self._output_filename_config.add_section('usrp_rx_hrpt')
		self._output_filename_config.set('usrp_rx_hrpt', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(self.config_filename)
		if not self._pll_alpha_config.has_section('usrp_rx_hrpt'):
			self._pll_alpha_config.add_section('usrp_rx_hrpt')
		self._pll_alpha_config.set('usrp_rx_hrpt', 'pll_alpha', str(self.pll_alpha))
		self._pll_alpha_config.write(open(self.config_filename, 'w'))
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_hrpt'):
			self._side_config.add_section('usrp_rx_hrpt')
		self._side_config.set('usrp_rx_hrpt', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(self.config_filename)
		if not self._decim_config.has_section('usrp_rx_hrpt'):
			self._decim_config.add_section('usrp_rx_hrpt')
		self._decim_config.set('usrp_rx_hrpt', 'decim', str(self.decim))
		self._decim_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_decim_rate(self.decim)

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

	def set_side(self, side):
		self.side = side
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_hrpt'):
			self._side_config.add_section('usrp_rx_hrpt')
		self._side_config.set('usrp_rx_hrpt', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

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

	def set_output_filename(self, output_filename):
		self.output_filename = output_filename
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('usrp_rx_hrpt'):
			self._output_filename_config.add_section('usrp_rx_hrpt')
		self._output_filename_config.set('usrp_rx_hrpt', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))

	def set_max_clock_offset(self, max_clock_offset):
		self.max_clock_offset = max_clock_offset

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.pll.set_max_offset(self.max_carrier_offset)

	def set_hs(self, hs):
		self.hs = hs
		self.gr_moving_average_xx_0.set_length_and_scale(self.hs, 1.0/self.hs)

	def set_gain(self, gain):
		self.gain = gain
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(self.config_filename)
		if not self._gain_config.has_section('usrp_rx_hrpt'):
			self._gain_config.add_section('usrp_rx_hrpt')
		self._gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._gain_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_gain(self.gain)

	def set_freq(self, freq):
		self.freq = freq
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(self.config_filename)
		if not self._freq_config.has_section('usrp_rx_hrpt'):
			self._freq_config.add_section('usrp_rx_hrpt')
		self._freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._freq_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_frequency(self.freq)

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self._clock_alpha_config = ConfigParser.ConfigParser()
		self._clock_alpha_config.read(self.config_filename)
		if not self._clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._clock_alpha_config.add_section('usrp_rx_hrpt')
		self._clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._clock_alpha_config.write(open(self.config_filename, 'w'))
		self.gr_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.gr_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	if gr.enable_realtime_scheduling() != gr.RT_OK:
		print "Error: failed to enable realtime scheduling."
	tb = usrp_rx_hrpt_nogui()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()

