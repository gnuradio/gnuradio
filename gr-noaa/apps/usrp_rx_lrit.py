#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP LRIT Receiver
# Generated: Wed Dec 16 09:49:15 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
from grc_gnuradio import usrp as grc_usrp
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import ConfigParser
import os
import wx

class usrp_rx_lrit(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="USRP LRIT Receiver")

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self._saved_decim_config = ConfigParser.ConfigParser()
		self._saved_decim_config.read(config_filename)
		try: saved_decim = self._saved_decim_config.getint('usrp_rx_lrit', 'decim')
		except: saved_decim = 160
		self.saved_decim = saved_decim
		self.decim = decim = saved_decim
		self.symbol_rate = symbol_rate = 293e3
		self._saved_offset_config = ConfigParser.ConfigParser()
		self._saved_offset_config.read(config_filename)
		try: saved_offset = self._saved_offset_config.getfloat('usrp_rx_lrit', 'offset')
		except: saved_offset = 0
		self.saved_offset = saved_offset
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(config_filename)
		try: saved_gain_mu = self._saved_gain_mu_config.getfloat('usrp_rx_lrit', 'gain_mu')
		except: saved_gain_mu = 0.005
		self.saved_gain_mu = saved_gain_mu
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(config_filename)
		try: saved_gain = self._saved_gain_config.getfloat('usrp_rx_lrit', 'gain')
		except: saved_gain = 33
		self.saved_gain = saved_gain
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(config_filename)
		try: saved_freq = self._saved_freq_config.getfloat('usrp_rx_lrit', 'freq')
		except: saved_freq = 1691e6
		self.saved_freq = saved_freq
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(config_filename)
		try: saved_costas_alpha = self._saved_costas_alpha_config.getfloat('usrp_rx_lrit', 'costas_alpha')
		except: saved_costas_alpha = 0.005
		self.saved_costas_alpha = saved_costas_alpha
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/symbol_rate
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(config_filename)
		try: side = self._side_config.get('usrp_rx_lrit', 'side')
		except: side = 'A'
		self.side = side
		self.offset = offset = saved_offset
		self.gain_mu = gain_mu = saved_gain_mu
		self.gain = gain = saved_gain
		self.freq = freq = saved_freq
		self.costas_alpha = costas_alpha = saved_costas_alpha

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "USRP RX")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "RRC")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "PLL")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Const")
		self.GridAdd(self.displays, 2, 0, 1, 3)

		##################################################
		# Controls
		##################################################
		self._decim_text_box = forms.text_box(
			parent=self.GetWin(),
			value=self.decim,
			callback=self.set_decim,
			label="Decim",
			converter=forms.int_converter(),
		)
		self.GridAdd(self._decim_text_box, 0, 2, 1, 1)
		_offset_sizer = wx.BoxSizer(wx.VERTICAL)
		self._offset_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_offset_sizer,
			value=self.offset,
			callback=self.set_offset,
			label="Offset",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._offset_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_offset_sizer,
			value=self.offset,
			callback=self.set_offset,
			minimum=-50e3,
			maximum=50e3,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_offset_sizer, 0, 1, 1, 1)
		_gain_mu_sizer = wx.BoxSizer(wx.VERTICAL)
		self._gain_mu_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_gain_mu_sizer,
			value=self.gain_mu,
			callback=self.set_gain_mu,
			label="Gain Mu",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._gain_mu_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_gain_mu_sizer,
			value=self.gain_mu,
			callback=self.set_gain_mu,
			minimum=0,
			maximum=0.5,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_gain_mu_sizer, 1, 2, 1, 1)
		_gain_sizer = wx.BoxSizer(wx.VERTICAL)
		self._gain_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_gain_sizer,
			value=self.gain,
			callback=self.set_gain,
			label="Gain",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._gain_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_gain_sizer,
			value=self.gain,
			callback=self.set_gain,
			minimum=0,
			maximum=115,
			num_steps=115,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_gain_sizer, 1, 0, 1, 1)
		self._freq_text_box = forms.text_box(
			parent=self.GetWin(),
			value=self.freq,
			callback=self.set_freq,
			label="Frequency",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._freq_text_box, 0, 0, 1, 1)
		_costas_alpha_sizer = wx.BoxSizer(wx.VERTICAL)
		self._costas_alpha_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_costas_alpha_sizer,
			value=self.costas_alpha,
			callback=self.set_costas_alpha,
			label="Costas Alpha",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._costas_alpha_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_costas_alpha_sizer,
			value=self.costas_alpha,
			callback=self.set_costas_alpha,
			minimum=0,
			maximum=0.5,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_costas_alpha_sizer, 1, 1, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.costas = gr.costas_loop_cc(costas_alpha, (costas_alpha**2.0)/4.0, 50e-6*sps, -50e-6*sps, 2)
		self.costas_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(2).GetWin(),
			baseband_freq=freq+offset,
			y_per_div=10,
			y_divs=10,
			ref_level=-15,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=15,
			average=False,
			avg_alpha=None,
			title="PLL Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(2).GridAdd(self.costas_spectrum.win, 0, 0, 1, 1)
		self.costas_waveform = scopesink2.scope_sink_c(
			self.displays.GetPage(2).GetWin(),
			title="PLL Waveform",
			sample_rate=sample_rate,
			v_scale=0.5,
			v_offset=0,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(2).GridAdd(self.costas_waveform.win, 1, 0, 1, 1)
		self.gr_agc_xx_0 = gr.agc_cc(1e-6, 1.0, 1.0/32767.0, 1.0)
		self.mm_const = scopesink2.scope_sink_c(
			self.displays.GetPage(3).GetWin(),
			title="Constellation",
			sample_rate=symbol_rate,
			v_scale=0.5,
			v_offset=0,
			t_scale=20.0/symbol_rate,
			ac_couple=False,
			xy_mode=True,
			num_inputs=1,
		)
		self.displays.GetPage(3).GridAdd(self.mm_const.win, 1, 0, 1, 1)
		self.mm_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(3).GetWin(),
			baseband_freq=0,
			y_per_div=10,
			y_divs=10,
			ref_level=-15,
			ref_scale=2.0,
			sample_rate=symbol_rate,
			fft_size=1024,
			fft_rate=15,
			average=False,
			avg_alpha=None,
			title="Bit Sync Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(3).GridAdd(self.mm_spectrum.win, 0, 0, 1, 1)
		self.mm_sync = gr.clock_recovery_mm_cc(sps, (gain_mu**2)/4.0, 0.5, gain_mu, 50e-6*sps)
		self.root_raised_cosine_filter_0 = gr.fir_filter_ccf(1, firdes.root_raised_cosine(
			1, sample_rate, symbol_rate, 0.25, int(11*sample_rate/symbol_rate)))
		self.rrc_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(1).GetWin(),
			baseband_freq=freq+offset,
			y_per_div=10,
			y_divs=10,
			ref_level=-15,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=15,
			average=False,
			avg_alpha=None,
			title="RRC Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(1).GridAdd(self.rrc_spectrum.win, 0, 0, 1, 1)
		self.rrc_waveform = scopesink2.scope_sink_c(
			self.displays.GetPage(1).GetWin(),
			title="RRC Waveform",
			sample_rate=sample_rate,
			v_scale=0.5,
			v_offset=0,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(1).GridAdd(self.rrc_waveform.win, 1, 0, 1, 1)
		self.rx_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=freq,
			y_per_div=10,
			y_divs=10,
			ref_level=50,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=30,
			average=False,
			avg_alpha=None,
			title="RX Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.rx_spectrum.win, 0, 0, 1, 1)
		self.rx_waveform = scopesink2.scope_sink_c(
			self.displays.GetPage(0).GetWin(),
			title="RX Waveform",
			sample_rate=sample_rate,
			v_scale=0,
			v_offset=0,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(0).GridAdd(self.rx_waveform.win, 1, 0, 1, 1)
		self.usrp_simple_source_x_0 = grc_usrp.simple_source_c(which=0, side=side, rx_ant="RXA")
		self.usrp_simple_source_x_0.set_decim_rate(decim)
		self.usrp_simple_source_x_0.set_frequency(freq+offset, verbose=True)
		self.usrp_simple_source_x_0.set_gain(gain)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_agc_xx_0, 0), (self.root_raised_cosine_filter_0, 0))
		self.connect((self.usrp_simple_source_x_0, 0), (self.gr_agc_xx_0, 0))
		self.connect((self.usrp_simple_source_x_0, 0), (self.rx_spectrum, 0))
		self.connect((self.usrp_simple_source_x_0, 0), (self.rx_waveform, 0))
		self.connect((self.root_raised_cosine_filter_0, 0), (self.rrc_waveform, 0))
		self.connect((self.root_raised_cosine_filter_0, 0), (self.rrc_spectrum, 0))
		self.connect((self.root_raised_cosine_filter_0, 0), (self.costas, 0))
		self.connect((self.costas, 0), (self.costas_spectrum, 0))
		self.connect((self.costas, 0), (self.costas_waveform, 0))
		self.connect((self.costas, 0), (self.mm_sync, 0))
		self.connect((self.mm_sync, 0), (self.mm_spectrum, 0))
		self.connect((self.mm_sync, 0), (self.mm_const, 0))

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section('usrp_rx_lrit'):
			self._saved_freq_config.add_section('usrp_rx_lrit')
		self._saved_freq_config.set('usrp_rx_lrit', 'freq', str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))
		self._saved_offset_config = ConfigParser.ConfigParser()
		self._saved_offset_config.read(self.config_filename)
		if not self._saved_offset_config.has_section('usrp_rx_lrit'):
			self._saved_offset_config.add_section('usrp_rx_lrit')
		self._saved_offset_config.set('usrp_rx_lrit', 'offset', str(self.offset))
		self._saved_offset_config.write(open(self.config_filename, 'w'))
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section('usrp_rx_lrit'):
			self._saved_gain_config.add_section('usrp_rx_lrit')
		self._saved_gain_config.set('usrp_rx_lrit', 'gain', str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))
		self._saved_decim_config = ConfigParser.ConfigParser()
		self._saved_decim_config.read(self.config_filename)
		if not self._saved_decim_config.has_section('usrp_rx_lrit'):
			self._saved_decim_config.add_section('usrp_rx_lrit')
		self._saved_decim_config.set('usrp_rx_lrit', 'decim', str(self.decim))
		self._saved_decim_config.write(open(self.config_filename, 'w'))
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(self.config_filename)
		if not self._saved_costas_alpha_config.has_section('usrp_rx_lrit'):
			self._saved_costas_alpha_config.add_section('usrp_rx_lrit')
		self._saved_costas_alpha_config.set('usrp_rx_lrit', 'costas_alpha', str(self.costas_alpha))
		self._saved_costas_alpha_config.write(open(self.config_filename, 'w'))
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(self.config_filename)
		if not self._saved_gain_mu_config.has_section('usrp_rx_lrit'):
			self._saved_gain_mu_config.add_section('usrp_rx_lrit')
		self._saved_gain_mu_config.set('usrp_rx_lrit', 'gain_mu', str(self.gain_mu))
		self._saved_gain_mu_config.write(open(self.config_filename, 'w'))
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_lrit'):
			self._side_config.add_section('usrp_rx_lrit')
		self._side_config.set('usrp_rx_lrit', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

	def set_saved_decim(self, saved_decim):
		self.saved_decim = saved_decim
		self.set_decim(self.saved_decim)

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)
		self._saved_decim_config = ConfigParser.ConfigParser()
		self._saved_decim_config.read(self.config_filename)
		if not self._saved_decim_config.has_section('usrp_rx_lrit'):
			self._saved_decim_config.add_section('usrp_rx_lrit')
		self._saved_decim_config.set('usrp_rx_lrit', 'decim', str(self.decim))
		self._saved_decim_config.write(open(self.config_filename, 'w'))
		self._decim_text_box.set_value(self.decim)
		self.usrp_simple_source_x_0.set_decim_rate(self.decim)

	def set_symbol_rate(self, symbol_rate):
		self.symbol_rate = symbol_rate
		self.set_sps(self.sample_rate/self.symbol_rate)
		self.root_raised_cosine_filter_0.set_taps(firdes.root_raised_cosine(1, self.sample_rate, self.symbol_rate, 0.25, int(11*self.sample_rate/self.symbol_rate)))
		self.mm_spectrum.set_sample_rate(self.symbol_rate)
		self.mm_const.set_sample_rate(self.symbol_rate)

	def set_saved_offset(self, saved_offset):
		self.saved_offset = saved_offset
		self.set_offset(self.saved_offset)

	def set_saved_gain_mu(self, saved_gain_mu):
		self.saved_gain_mu = saved_gain_mu
		self.set_gain_mu(self.saved_gain_mu)

	def set_saved_gain(self, saved_gain):
		self.saved_gain = saved_gain
		self.set_gain(self.saved_gain)

	def set_saved_freq(self, saved_freq):
		self.saved_freq = saved_freq
		self.set_freq(self.saved_freq)

	def set_saved_costas_alpha(self, saved_costas_alpha):
		self.saved_costas_alpha = saved_costas_alpha
		self.set_costas_alpha(self.saved_costas_alpha)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_sps(self.sample_rate/self.symbol_rate)
		self.rx_waveform.set_sample_rate(self.sample_rate)
		self.rx_spectrum.set_sample_rate(self.sample_rate)
		self.root_raised_cosine_filter_0.set_taps(firdes.root_raised_cosine(1, self.sample_rate, self.symbol_rate, 0.25, int(11*self.sample_rate/self.symbol_rate)))
		self.rrc_waveform.set_sample_rate(self.sample_rate)
		self.rrc_spectrum.set_sample_rate(self.sample_rate)
		self.costas_spectrum.set_sample_rate(self.sample_rate)
		self.costas_waveform.set_sample_rate(self.sample_rate)

	def set_sps(self, sps):
		self.sps = sps
		self.mm_sync.set_omega(self.sps)

	def set_side(self, side):
		self.side = side
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_lrit'):
			self._side_config.add_section('usrp_rx_lrit')
		self._side_config.set('usrp_rx_lrit', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

	def set_offset(self, offset):
		self.offset = offset
		self._saved_offset_config = ConfigParser.ConfigParser()
		self._saved_offset_config.read(self.config_filename)
		if not self._saved_offset_config.has_section('usrp_rx_lrit'):
			self._saved_offset_config.add_section('usrp_rx_lrit')
		self._saved_offset_config.set('usrp_rx_lrit', 'offset', str(self.offset))
		self._saved_offset_config.write(open(self.config_filename, 'w'))
		self._offset_slider.set_value(self.offset)
		self._offset_text_box.set_value(self.offset)
		self.rrc_spectrum.set_baseband_freq(self.freq+self.offset)
		self.costas_spectrum.set_baseband_freq(self.freq+self.offset)
		self.usrp_simple_source_x_0.set_frequency(self.freq+self.offset)

	def set_gain_mu(self, gain_mu):
		self.gain_mu = gain_mu
		self._gain_mu_slider.set_value(self.gain_mu)
		self._gain_mu_text_box.set_value(self.gain_mu)
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(self.config_filename)
		if not self._saved_gain_mu_config.has_section('usrp_rx_lrit'):
			self._saved_gain_mu_config.add_section('usrp_rx_lrit')
		self._saved_gain_mu_config.set('usrp_rx_lrit', 'gain_mu', str(self.gain_mu))
		self._saved_gain_mu_config.write(open(self.config_filename, 'w'))
		self.mm_sync.set_gain_omega((self.gain_mu**2)/4.0)
		self.mm_sync.set_gain_mu(self.gain_mu)

	def set_gain(self, gain):
		self.gain = gain
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section('usrp_rx_lrit'):
			self._saved_gain_config.add_section('usrp_rx_lrit')
		self._saved_gain_config.set('usrp_rx_lrit', 'gain', str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))
		self._gain_slider.set_value(self.gain)
		self._gain_text_box.set_value(self.gain)
		self.usrp_simple_source_x_0.set_gain(self.gain)

	def set_freq(self, freq):
		self.freq = freq
		self._freq_text_box.set_value(self.freq)
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section('usrp_rx_lrit'):
			self._saved_freq_config.add_section('usrp_rx_lrit')
		self._saved_freq_config.set('usrp_rx_lrit', 'freq', str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))
		self.rx_spectrum.set_baseband_freq(self.freq)
		self.rrc_spectrum.set_baseband_freq(self.freq+self.offset)
		self.costas_spectrum.set_baseband_freq(self.freq+self.offset)
		self.usrp_simple_source_x_0.set_frequency(self.freq+self.offset)

	def set_costas_alpha(self, costas_alpha):
		self.costas_alpha = costas_alpha
		self._costas_alpha_slider.set_value(self.costas_alpha)
		self._costas_alpha_text_box.set_value(self.costas_alpha)
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(self.config_filename)
		if not self._saved_costas_alpha_config.has_section('usrp_rx_lrit'):
			self._saved_costas_alpha_config.add_section('usrp_rx_lrit')
		self._saved_costas_alpha_config.set('usrp_rx_lrit', 'costas_alpha', str(self.costas_alpha))
		self._saved_costas_alpha_config.write(open(self.config_filename, 'w'))
		self.costas.set_alpha(self.costas_alpha)
		self.costas.set_beta((self.costas_alpha**2.0)/4.0)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_lrit()
	tb.Run(True)

