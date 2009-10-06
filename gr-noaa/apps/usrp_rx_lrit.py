#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP LRIT Receiver
# Generated: Sat Aug 29 11:26:03 2009
##################################################

from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import numbersink2
from gnuradio.wxgui import scopesink2
from grc_gnuradio import blks2 as grc_blks2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import ConfigParser
import wx

class usrp_rx_lrit(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="USRP LRIT Receiver")

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = "usrp_rx_lrit.cfg"
		self._saved_decim_config = ConfigParser.ConfigParser()
		self._saved_decim_config.read(config_filename)
		try: saved_decim = self._saved_decim_config.getint("main", "decim")
		except: saved_decim = 160
		self.saved_decim = saved_decim
		self.decim = decim = saved_decim
		self.symbol_rate = symbol_rate = 293e3
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(config_filename)
		try: saved_gain_mu = self._saved_gain_mu_config.getfloat("main", "gain_mu")
		except: saved_gain_mu = 0.005
		self.saved_gain_mu = saved_gain_mu
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(config_filename)
		try: saved_gain = self._saved_gain_config.getfloat("main", "gain")
		except: saved_gain = 33
		self.saved_gain = saved_gain
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(config_filename)
		try: saved_freq = self._saved_freq_config.getfloat("main", "freq")
		except: saved_freq = 137e6
		self.saved_freq = saved_freq
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(config_filename)
		try: saved_costas_alpha = self._saved_costas_alpha_config.getfloat("main", "costas_alpha")
		except: saved_costas_alpha = 0.005
		self.saved_costas_alpha = saved_costas_alpha
		self.samp_rate = samp_rate = 64e6/decim
		self.sps = sps = samp_rate/symbol_rate
		self.gain_mu = gain_mu = saved_gain_mu
		self.gain = gain = saved_gain
		self.freq = freq = saved_freq
		self.costas_alpha = costas_alpha = saved_costas_alpha

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "USRP RX")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Costas Output")
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
		self.GridAdd(self._decim_text_box, 0, 0, 1, 1)
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
		self.GridAdd(_gain_mu_sizer, 1, 1, 1, 1)
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
		self.GridAdd(_gain_sizer, 0, 1, 1, 1)
		_freq_sizer = wx.BoxSizer(wx.VERTICAL)
		self._freq_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_freq_sizer,
			value=self.freq,
			callback=self.set_freq,
			label="Frequency",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._freq_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_freq_sizer,
			value=self.freq,
			callback=self.set_freq,
			minimum=135e6,
			maximum=139e6,
			num_steps=400,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_freq_sizer, 0, 2, 1, 1)
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
		self.GridAdd(_costas_alpha_sizer, 1, 0, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.gr_add_const_vxx_0 = gr.add_const_vff((48.0, ))
		self.gr_agc_xx_0 = gr.agc_cc(1e-6, 1.0, 1.0/32767.0, 1.0)
		self.gr_binary_slicer_fb_0 = gr.binary_slicer_fb()
		self.gr_char_to_float_0 = gr.char_to_float()
		self.gr_complex_to_real_0 = gr.complex_to_real(1)
		self.gr_file_sink_0 = gr.file_sink(gr.sizeof_char*1, "bits.dat")
		self.gr_file_source_0 = gr.file_source(gr.sizeof_gr_complex*1, "lrit.dat", False)
		self.gr_float_to_char_0 = gr.float_to_char()
		self.gr_mpsk_receiver_cc_0 = gr.mpsk_receiver_cc(2, 0, costas_alpha, costas_alpha*costas_alpha/4.0, -0.05, 0.05, 0.5, gain_mu, sps, gain_mu*gain_mu/4.0, 0.05)
		self.gr_probe_mpsk_snr_c_0 = grc_blks2.probe_mpsk_snr_c(
			type='snr',
			alpha=0.0001,
			probe_rate=10,
		)
		self.gr_throttle_0 = gr.throttle(gr.sizeof_gr_complex*1, samp_rate)
		self.wxgui_fftsink2_0 = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=freq,
			y_per_div=10,
			y_divs=10,
			ref_level=50,
			sample_rate=samp_rate,
			fft_size=1024,
			fft_rate=30,
			average=False,
			avg_alpha=None,
			title="Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.wxgui_fftsink2_0.win, 0, 0, 1, 1)
		self.wxgui_numbersink2_0 = numbersink2.number_sink_f(
			self.displays.GetPage(1).GetWin(),
			unit="dB",
			minval=0,
			maxval=30,
			factor=1.0,
			decimal_places=1,
			ref_level=0,
			sample_rate=10,
			number_rate=10,
			average=False,
			avg_alpha=None,
			label="SNR",
			peak_hold=False,
			show_gauge=True,
		)
		self.displays.GetPage(1).GridAdd(self.wxgui_numbersink2_0.win, 2, 0, 1, 1)
		self.wxgui_scopesink2_0 = scopesink2.scope_sink_c(
			self.displays.GetPage(0).GetWin(),
			title="Waveform",
			sample_rate=samp_rate,
			v_scale=0.5,
			t_scale=20.0/samp_rate,
			ac_couple=False,
			xy_mode=True,
			num_inputs=1,
		)
		self.displays.GetPage(0).GridAdd(self.wxgui_scopesink2_0.win, 1, 0, 1, 1)
		self.wxgui_scopesink2_1 = scopesink2.scope_sink_c(
			self.displays.GetPage(1).GetWin(),
			title="Scope Plot",
			sample_rate=samp_rate,
			v_scale=0.4,
			t_scale=20.0/samp_rate,
			ac_couple=False,
			xy_mode=True,
			num_inputs=1,
		)
		self.displays.GetPage(1).GridAdd(self.wxgui_scopesink2_1.win, 0, 0, 1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_agc_xx_0, 0), (self.wxgui_scopesink2_0, 0))
		self.connect((self.gr_file_source_0, 0), (self.gr_throttle_0, 0))
		self.connect((self.gr_throttle_0, 0), (self.gr_agc_xx_0, 0))
		self.connect((self.gr_probe_mpsk_snr_c_0, 0), (self.wxgui_numbersink2_0, 0))
		self.connect((self.gr_mpsk_receiver_cc_0, 0), (self.gr_probe_mpsk_snr_c_0, 0))
		self.connect((self.gr_agc_xx_0, 0), (self.gr_mpsk_receiver_cc_0, 0))
		self.connect((self.gr_mpsk_receiver_cc_0, 0), (self.wxgui_scopesink2_1, 0))
		self.connect((self.gr_agc_xx_0, 0), (self.wxgui_fftsink2_0, 0))
		self.connect((self.gr_mpsk_receiver_cc_0, 0), (self.gr_complex_to_real_0, 0))
		self.connect((self.gr_complex_to_real_0, 0), (self.gr_binary_slicer_fb_0, 0))
		self.connect((self.gr_binary_slicer_fb_0, 0), (self.gr_char_to_float_0, 0))
		self.connect((self.gr_char_to_float_0, 0), (self.gr_add_const_vxx_0, 0))
		self.connect((self.gr_add_const_vxx_0, 0), (self.gr_float_to_char_0, 0))
		self.connect((self.gr_float_to_char_0, 0), (self.gr_file_sink_0, 0))

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._saved_decim_config = ConfigParser.ConfigParser()
		self._saved_decim_config.read(self.config_filename)
		if not self._saved_decim_config.has_section("main"):
			self._saved_decim_config.add_section("main")
		self._saved_decim_config.set("main", "decim", str(self.decim))
		self._saved_decim_config.write(open(self.config_filename, 'w'))
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section("main"):
			self._saved_gain_config.add_section("main")
		self._saved_gain_config.set("main", "gain", str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section("main"):
			self._saved_freq_config.add_section("main")
		self._saved_freq_config.set("main", "freq", str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(self.config_filename)
		if not self._saved_costas_alpha_config.has_section("main"):
			self._saved_costas_alpha_config.add_section("main")
		self._saved_costas_alpha_config.set("main", "costas_alpha", str(self.costas_alpha))
		self._saved_costas_alpha_config.write(open(self.config_filename, 'w'))
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(self.config_filename)
		if not self._saved_gain_mu_config.has_section("main"):
			self._saved_gain_mu_config.add_section("main")
		self._saved_gain_mu_config.set("main", "gain_mu", str(self.gain_mu))
		self._saved_gain_mu_config.write(open(self.config_filename, 'w'))

	def set_saved_decim(self, saved_decim):
		self.saved_decim = saved_decim
		self.set_decim(self.saved_decim)

	def set_decim(self, decim):
		self.decim = decim
		self.set_samp_rate(64e6/self.decim)
		self._decim_text_box.set_value(self.decim)
		self._saved_decim_config = ConfigParser.ConfigParser()
		self._saved_decim_config.read(self.config_filename)
		if not self._saved_decim_config.has_section("main"):
			self._saved_decim_config.add_section("main")
		self._saved_decim_config.set("main", "decim", str(self.decim))
		self._saved_decim_config.write(open(self.config_filename, 'w'))

	def set_symbol_rate(self, symbol_rate):
		self.symbol_rate = symbol_rate
		self.set_sps(self.samp_rate/self.symbol_rate)

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

	def set_samp_rate(self, samp_rate):
		self.samp_rate = samp_rate
		self.set_sps(self.samp_rate/self.symbol_rate)
		self.wxgui_scopesink2_0.set_sample_rate(self.samp_rate)
		self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate)
		self.wxgui_scopesink2_1.set_sample_rate(self.samp_rate)

	def set_sps(self, sps):
		self.sps = sps
		self.gr_mpsk_receiver_cc_0.set_omega(self.sps)

	def set_gain_mu(self, gain_mu):
		self.gain_mu = gain_mu
		self.gr_mpsk_receiver_cc_0.set_gain_mu(self.gain_mu)
		self.gr_mpsk_receiver_cc_0.set_gain_omega(self.gain_mu*self.gain_mu/4.0)
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(self.config_filename)
		if not self._saved_gain_mu_config.has_section("main"):
			self._saved_gain_mu_config.add_section("main")
		self._saved_gain_mu_config.set("main", "gain_mu", str(self.gain_mu))
		self._saved_gain_mu_config.write(open(self.config_filename, 'w'))
		self._gain_mu_slider.set_value(self.gain_mu)
		self._gain_mu_text_box.set_value(self.gain_mu)

	def set_gain(self, gain):
		self.gain = gain
		self._gain_slider.set_value(self.gain)
		self._gain_text_box.set_value(self.gain)
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section("main"):
			self._saved_gain_config.add_section("main")
		self._saved_gain_config.set("main", "gain", str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))

	def set_freq(self, freq):
		self.freq = freq
		self.wxgui_fftsink2_0.set_baseband_freq(self.freq)
		self._freq_slider.set_value(self.freq)
		self._freq_text_box.set_value(self.freq)
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section("main"):
			self._saved_freq_config.add_section("main")
		self._saved_freq_config.set("main", "freq", str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))

	def set_costas_alpha(self, costas_alpha):
		self.costas_alpha = costas_alpha
		self.gr_mpsk_receiver_cc_0.set_alpha(self.costas_alpha)
		self.gr_mpsk_receiver_cc_0.set_beta(self.costas_alpha*self.costas_alpha/4.0)
		self._costas_alpha_slider.set_value(self.costas_alpha)
		self._costas_alpha_text_box.set_value(self.costas_alpha)
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(self.config_filename)
		if not self._saved_costas_alpha_config.has_section("main"):
			self._saved_costas_alpha_config.add_section("main")
		self._saved_costas_alpha_config.set("main", "costas_alpha", str(self.costas_alpha))
		self._saved_costas_alpha_config.write(open(self.config_filename, 'w'))

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_lrit()
	tb.Run(True)

