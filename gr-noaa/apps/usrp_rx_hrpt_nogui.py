#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Sun Sep 27 10:13:25 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from grc_gnuradio import usrp as grc_usrp
from optparse import OptionParser
import ConfigParser
import math

class usrp_rx_hrpt_nogui(gr.top_block):

	def __init__(self):
		gr.top_block.__init__(self, "USRP HRPT Receiver")

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = 'usrp_rx_hrpt.cfg'
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(config_filename)
		try: decim = self._decim_config.getfloat('usrp', 'decim')
		except: decim = 16
		self.decim = decim
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self.hs = hs = int(sps/2.0)
		self._sync_alpha_config = ConfigParser.ConfigParser()
		self._sync_alpha_config.read(config_filename)
		try: sync_alpha = self._sync_alpha_config.getfloat('demod', 'sync_alpha')
		except: sync_alpha = 0.05
		self.sync_alpha = sync_alpha
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(config_filename)
		try: side = self._side_config.get('usrp', 'side')
		except: side = 'A'
		self.side = side
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(config_filename)
		try: pll_alpha = self._pll_alpha_config.getfloat('demod', 'pll_alpha')
		except: pll_alpha = 0.05
		self.pll_alpha = pll_alpha
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(config_filename)
		try: output_filename = self._output_filename_config.get('output', 'filename')
		except: output_filename = 'frames.dat'
		self.output_filename = output_filename
		self.mf_taps = mf_taps = [-0.5/hs,]*hs+[0.5/hs,]*hs
		self.max_sync_offset = max_sync_offset = 0.01
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(config_filename)
		try: gain = self._gain_config.getfloat('usrp', 'gain')
		except: gain = 35
		self.gain = gain
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(config_filename)
		try: freq = self._freq_config.getfloat('usrp', 'freq')
		except: freq = 1698e6
		self.freq = freq

		##################################################
		# Blocks
		##################################################
		self.agc = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)
		self.decoder = noaa.hrpt_decoder()
		self.deframer = noaa.hrpt_deframer()
		self.frame_sink = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.gr_fir_filter_xxx_0 = gr.fir_filter_ccc(1, (mf_taps))
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.sync = noaa.hrpt_sync_fb(sync_alpha, sync_alpha**2/4.0, sps, max_sync_offset)
		self.usrp_source = grc_usrp.simple_source_c(which=0, side=side, rx_ant="RXA")
		self.usrp_source.set_decim_rate(decim)
		self.usrp_source.set_frequency(freq, verbose=True)
		self.usrp_source.set_gain(gain)

		##################################################
		# Connections
		##################################################
		self.connect((self.usrp_source, 0), (self.agc, 0))
		self.connect((self.deframer, 0), (self.decoder, 0))
		self.connect((self.pll, 0), (self.sync, 0))
		self.connect((self.sync, 0), (self.deframer, 0))
		self.connect((self.deframer, 0), (self.frame_sink, 0))
		self.connect((self.agc, 0), (self.gr_fir_filter_xxx_0, 0))
		self.connect((self.gr_fir_filter_xxx_0, 0), (self.pll, 0))

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('output'):
			self._output_filename_config.add_section('output')
		self._output_filename_config.set('output', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))
		self._sync_alpha_config = ConfigParser.ConfigParser()
		self._sync_alpha_config.read(self.config_filename)
		if not self._sync_alpha_config.has_section('demod'):
			self._sync_alpha_config.add_section('demod')
		self._sync_alpha_config.set('demod', 'sync_alpha', str(self.sync_alpha))
		self._sync_alpha_config.write(open(self.config_filename, 'w'))
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(self.config_filename)
		if not self._pll_alpha_config.has_section('demod'):
			self._pll_alpha_config.add_section('demod')
		self._pll_alpha_config.set('demod', 'pll_alpha', str(self.pll_alpha))
		self._pll_alpha_config.write(open(self.config_filename, 'w'))
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(self.config_filename)
		if not self._gain_config.has_section('usrp'):
			self._gain_config.add_section('usrp')
		self._gain_config.set('usrp', 'gain', str(self.gain))
		self._gain_config.write(open(self.config_filename, 'w'))
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(self.config_filename)
		if not self._freq_config.has_section('usrp'):
			self._freq_config.add_section('usrp')
		self._freq_config.set('usrp', 'freq', str(self.freq))
		self._freq_config.write(open(self.config_filename, 'w'))
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(self.config_filename)
		if not self._decim_config.has_section('usrp'):
			self._decim_config.add_section('usrp')
		self._decim_config.set('usrp', 'decim', str(self.decim))
		self._decim_config.write(open(self.config_filename, 'w'))
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp'):
			self._side_config.add_section('usrp')
		self._side_config.set('usrp', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(self.config_filename)
		if not self._decim_config.has_section('usrp'):
			self._decim_config.add_section('usrp')
		self._decim_config.set('usrp', 'decim', str(self.decim))
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

	def set_hs(self, hs):
		self.hs = hs
		self.set_mf_taps([-0.5/self.hs,]*self.hs+[0.5/self.hs,]*self.hs)

	def set_sync_alpha(self, sync_alpha):
		self.sync_alpha = sync_alpha
		self._sync_alpha_config = ConfigParser.ConfigParser()
		self._sync_alpha_config.read(self.config_filename)
		if not self._sync_alpha_config.has_section('demod'):
			self._sync_alpha_config.add_section('demod')
		self._sync_alpha_config.set('demod', 'sync_alpha', str(self.sync_alpha))
		self._sync_alpha_config.write(open(self.config_filename, 'w'))
		self.sync.set_alpha(self.sync_alpha)
		self.sync.set_beta(self.sync_alpha**2/4.0)

	def set_side(self, side):
		self.side = side
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp'):
			self._side_config.add_section('usrp')
		self._side_config.set('usrp', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self._pll_alpha_config = ConfigParser.ConfigParser()
		self._pll_alpha_config.read(self.config_filename)
		if not self._pll_alpha_config.has_section('demod'):
			self._pll_alpha_config.add_section('demod')
		self._pll_alpha_config.set('demod', 'pll_alpha', str(self.pll_alpha))
		self._pll_alpha_config.write(open(self.config_filename, 'w'))
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)

	def set_output_filename(self, output_filename):
		self.output_filename = output_filename
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('output'):
			self._output_filename_config.add_section('output')
		self._output_filename_config.set('output', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))

	def set_mf_taps(self, mf_taps):
		self.mf_taps = mf_taps
		self.gr_fir_filter_xxx_0.set_taps((self.mf_taps))

	def set_max_sync_offset(self, max_sync_offset):
		self.max_sync_offset = max_sync_offset
		self.sync.set_max_offset(self.max_sync_offset)

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.pll.set_max_offset(self.max_carrier_offset)

	def set_gain(self, gain):
		self.gain = gain
		self._gain_config = ConfigParser.ConfigParser()
		self._gain_config.read(self.config_filename)
		if not self._gain_config.has_section('usrp'):
			self._gain_config.add_section('usrp')
		self._gain_config.set('usrp', 'gain', str(self.gain))
		self._gain_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_gain(self.gain)

	def set_freq(self, freq):
		self.freq = freq
		self._freq_config = ConfigParser.ConfigParser()
		self._freq_config.read(self.config_filename)
		if not self._freq_config.has_section('usrp'):
			self._freq_config.add_section('usrp')
		self._freq_config.set('usrp', 'freq', str(self.freq))
		self._freq_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_frequency(self.freq)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	if gr.enable_realtime_scheduling() != gr.RT_OK:
		print "Error: failed to enable realtime scheduling."
	tb = usrp_rx_hrpt_nogui()
	tb.start()
	raw_input('Press Enter to quit: ')
	tb.stop()

