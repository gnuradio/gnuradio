#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: LRIT Receiver (from capture file)
# Generated: Wed Dec 16 09:16:13 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import ConfigParser
import os
import wx

class file_rx_lrit(grc_wxgui.top_block_gui):

	def __init__(self, decim=160):
		grc_wxgui.top_block_gui.__init__(self, title="LRIT Receiver (from capture file)")

		##################################################
		# Parameters
		##################################################
		self.decim = decim

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self.symbol_rate = symbol_rate = 293e3
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(config_filename)
		try: saved_gain_mu = self._saved_gain_mu_config.getfloat('usrp_rx_lrit', 'gain_mu')
		except: saved_gain_mu = 0.2
		self.saved_gain_mu = saved_gain_mu
		self._saved_costas_alpha_config = ConfigParser.ConfigParser()
		self._saved_costas_alpha_config.read(config_filename)
		try: saved_costas_alpha = self._saved_costas_alpha_config.getfloat('usrp_rx_lrit', 'costas_alpha')
		except: saved_costas_alpha = 0.2
		self.saved_costas_alpha = saved_costas_alpha
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/symbol_rate
		self.gain_mu = gain_mu = saved_gain_mu
		self.costas_alpha = costas_alpha = saved_costas_alpha

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "USRP RX")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "RRC Filter")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "PLL")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "MM")
		self.GridAdd(self.displays, 1, 0, 1, 2)

		##################################################
		# Controls
		##################################################
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
		self.GridAdd(_gain_mu_sizer, 0, 1, 1, 1)
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
		self.GridAdd(_costas_alpha_sizer, 0, 0, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.costas = gr.costas_loop_cc(costas_alpha, (costas_alpha**2)/4.0, 50e-6*sps, -50e-6*sps, 2)
		self.costas_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(2).GetWin(),
			baseband_freq=0,
			y_per_div=10,
			y_divs=10,
			ref_level=-15,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=30,
			average=True,
			avg_alpha=None,
			title="PLL Spectrum",
			peak_hold=False,
			win=window.hanning,
		)
		self.displays.GetPage(2).GridAdd(self.costas_spectrum.win, 0, 0, 1, 1)
		self.costas_spectrum_0 = fftsink2.fft_sink_c(
			self.displays.GetPage(3).GetWin(),
			baseband_freq=0,
			y_per_div=10,
			y_divs=10,
			ref_level=-15,
			ref_scale=2.0,
			sample_rate=symbol_rate,
			fft_size=1024,
			fft_rate=30,
			average=True,
			avg_alpha=None,
			title="Bit Sync Spectrum",
			peak_hold=False,
			win=window.hanning,
		)
		self.displays.GetPage(3).GridAdd(self.costas_spectrum_0.win, 0, 0, 1, 1)
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
		self.gr_clock_recovery_mm_xx_0 = gr.clock_recovery_mm_cc(sps, (gain_mu**2)/4.0, 0.5, gain_mu, 50e-6)
		self.gr_file_source_0 = gr.file_source(gr.sizeof_gr_complex*1, "lrit.dat", False)
		self.gr_throttle_0 = gr.throttle(gr.sizeof_gr_complex*1, sample_rate)
		self.mm_waveform = scopesink2.scope_sink_c(
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
		self.displays.GetPage(3).GridAdd(self.mm_waveform.win, 1, 0, 1, 1)
		self.rrc = gr.fir_filter_ccf(1, firdes.root_raised_cosine(
			1, sample_rate, symbol_rate, 0.25, int(11*sample_rate/symbol_rate)))
		self.rrc_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(1).GetWin(),
			baseband_freq=0,
			y_per_div=10,
			y_divs=10,
			ref_level=-15,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=30,
			average=True,
			avg_alpha=None,
			title="RRC Spectrum",
			peak_hold=False,
			win=window.hanning,
		)
		self.displays.GetPage(1).GridAdd(self.rrc_spectrum.win, 0, 0, 1, 1)
		self.rx_spectrum = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=0,
			y_per_div=5,
			y_divs=10,
			ref_level=50,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=15,
			average=True,
			avg_alpha=None,
			title="Baseband Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.rx_spectrum.win, 0, 0, 1, 1)
		self.rx_waveform = scopesink2.scope_sink_c(
			self.displays.GetPage(0).GetWin(),
			title="Baseband Waveform",
			sample_rate=sample_rate,
			v_scale=0,
			v_offset=0,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(0).GridAdd(self.rx_waveform.win, 1, 0, 1, 1)
		self.rx_waveform_0 = scopesink2.scope_sink_c(
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
		self.displays.GetPage(1).GridAdd(self.rx_waveform_0.win, 1, 0, 1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.rrc, 0), (self.rx_waveform_0, 0))
		self.connect((self.rrc, 0), (self.rrc_spectrum, 0))
		self.connect((self.gr_agc_xx_0, 0), (self.rrc, 0))
		self.connect((self.gr_throttle_0, 0), (self.rx_waveform, 0))
		self.connect((self.gr_throttle_0, 0), (self.rx_spectrum, 0))
		self.connect((self.gr_throttle_0, 0), (self.gr_agc_xx_0, 0))
		self.connect((self.gr_file_source_0, 0), (self.gr_throttle_0, 0))
		self.connect((self.rrc, 0), (self.costas, 0))
		self.connect((self.costas, 0), (self.costas_spectrum, 0))
		self.connect((self.costas, 0), (self.costas_waveform, 0))
		self.connect((self.costas, 0), (self.gr_clock_recovery_mm_xx_0, 0))
		self.connect((self.gr_clock_recovery_mm_xx_0, 0), (self.costas_spectrum_0, 0))
		self.connect((self.gr_clock_recovery_mm_xx_0, 0), (self.mm_waveform, 0))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
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

	def set_symbol_rate(self, symbol_rate):
		self.symbol_rate = symbol_rate
		self.set_sps(self.sample_rate/self.symbol_rate)
		self.costas_spectrum_0.set_sample_rate(self.symbol_rate)
		self.mm_waveform.set_sample_rate(self.symbol_rate)
		self.rrc.set_taps(firdes.root_raised_cosine(1, self.sample_rate, self.symbol_rate, 0.25, int(11*self.sample_rate/self.symbol_rate)))

	def set_saved_gain_mu(self, saved_gain_mu):
		self.saved_gain_mu = saved_gain_mu
		self.set_gain_mu(self.saved_gain_mu)

	def set_saved_costas_alpha(self, saved_costas_alpha):
		self.saved_costas_alpha = saved_costas_alpha
		self.set_costas_alpha(self.saved_costas_alpha)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_sps(self.sample_rate/self.symbol_rate)
		self.rx_spectrum.set_sample_rate(self.sample_rate)
		self.rx_waveform.set_sample_rate(self.sample_rate)
		self.costas_spectrum.set_sample_rate(self.sample_rate)
		self.costas_waveform.set_sample_rate(self.sample_rate)
		self.rrc_spectrum.set_sample_rate(self.sample_rate)
		self.rx_waveform_0.set_sample_rate(self.sample_rate)
		self.rrc.set_taps(firdes.root_raised_cosine(1, self.sample_rate, self.symbol_rate, 0.25, int(11*self.sample_rate/self.symbol_rate)))

	def set_sps(self, sps):
		self.sps = sps
		self.gr_clock_recovery_mm_xx_0.set_omega(self.sps)

	def set_gain_mu(self, gain_mu):
		self.gain_mu = gain_mu
		self._gain_mu_slider.set_value(self.gain_mu)
		self._gain_mu_text_box.set_value(self.gain_mu)
		self.gr_clock_recovery_mm_xx_0.set_gain_omega((self.gain_mu**2)/4.0)
		self.gr_clock_recovery_mm_xx_0.set_gain_mu(self.gain_mu)
		self._saved_gain_mu_config = ConfigParser.ConfigParser()
		self._saved_gain_mu_config.read(self.config_filename)
		if not self._saved_gain_mu_config.has_section('usrp_rx_lrit'):
			self._saved_gain_mu_config.add_section('usrp_rx_lrit')
		self._saved_gain_mu_config.set('usrp_rx_lrit', 'gain_mu', str(self.gain_mu))
		self._saved_gain_mu_config.write(open(self.config_filename, 'w'))

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
		self.costas.set_beta((self.costas_alpha**2)/4.0)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=160,
		help="Set Decim [default=%default]")
	(options, args) = parser.parse_args()
	tb = file_rx_lrit(decim=options.decim)
	tb.Run(True)

