#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Mon Nov  9 07:56:11 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
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
import math, os
import wx

class usrp_rx_hrpt(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="USRP HRPT Receiver")

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(config_filename)
		try: decim = self._decim_config.getfloat('usrp_rx_hrpt', 'decim')
		except: decim = 32
		self.decim = decim
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(config_filename)
		try: side = self._side_config.get('usrp_rx_hrpt', 'side')
		except: side = 'A'
		self.side = side
		self._saved_pll_alpha_config = ConfigParser.ConfigParser()
		self._saved_pll_alpha_config.read(config_filename)
		try: saved_pll_alpha = self._saved_pll_alpha_config.getfloat('usrp_rx_hrpt', 'pll_alpha')
		except: saved_pll_alpha = 0.01
		self.saved_pll_alpha = saved_pll_alpha
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(config_filename)
		try: saved_gain = self._saved_gain_config.getfloat('usrp_rx_hrpt', 'gain')
		except: saved_gain = 35
		self.saved_gain = saved_gain
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(config_filename)
		try: saved_freq = self._saved_freq_config.getfloat('usrp_rx_hrpt', 'freq')
		except: saved_freq = 1698e6
		self.saved_freq = saved_freq
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(config_filename)
		try: saved_clock_alpha = self._saved_clock_alpha_config.getfloat('usrp_rx_hrpt', 'clock_alpha')
		except: saved_clock_alpha = 0.01
		self.saved_clock_alpha = saved_clock_alpha
		self.side_text = side_text = side
		self.pll_alpha = pll_alpha = saved_pll_alpha
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(config_filename)
		try: output_filename = self._output_filename_config.get('usrp_rx_hrpt', 'filename')
		except: output_filename = 'frames.hrpt'
		self.output_filename = output_filename
		self.max_clock_offset = max_clock_offset = 100e-6
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)
		self.gain = gain = saved_gain
		self.freq = freq = saved_freq
		self.decim_text = decim_text = decim
		self.clock_alpha = clock_alpha = saved_clock_alpha

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Spectrum")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Demod")
		self.GridAdd(self.displays, 2, 0, 1, 4)

		##################################################
		# Controls
		##################################################
		self._side_text_static_text = forms.static_text(
			parent=self.GetWin(),
			value=self.side_text,
			callback=self.set_side_text,
			label="USRP Side",
			converter=forms.str_converter(),
		)
		self.GridAdd(self._side_text_static_text, 1, 0, 1, 1)
		_pll_alpha_sizer = wx.BoxSizer(wx.VERTICAL)
		self._pll_alpha_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_pll_alpha_sizer,
			value=self.pll_alpha,
			callback=self.set_pll_alpha,
			label="PLL Alpha",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._pll_alpha_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_pll_alpha_sizer,
			value=self.pll_alpha,
			callback=self.set_pll_alpha,
			minimum=0.0,
			maximum=0.5,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_pll_alpha_sizer, 0, 2, 1, 1)
		_gain_sizer = wx.BoxSizer(wx.VERTICAL)
		self._gain_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_gain_sizer,
			value=self.gain,
			callback=self.set_gain,
			label="RX Gain",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._gain_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_gain_sizer,
			value=self.gain,
			callback=self.set_gain,
			minimum=0,
			maximum=100,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_gain_sizer, 0, 1, 1, 1)
		self._freq_text_box = forms.text_box(
			parent=self.GetWin(),
			value=self.freq,
			callback=self.set_freq,
			label="Frequency",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._freq_text_box, 0, 0, 1, 1)
		self._decim_text_static_text = forms.static_text(
			parent=self.GetWin(),
			value=self.decim_text,
			callback=self.set_decim_text,
			label="Decimation",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._decim_text_static_text, 1, 1, 1, 1)
		_clock_alpha_sizer = wx.BoxSizer(wx.VERTICAL)
		self._clock_alpha_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_clock_alpha_sizer,
			value=self.clock_alpha,
			callback=self.set_clock_alpha,
			label="Clock Alpha",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._clock_alpha_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_clock_alpha_sizer,
			value=self.clock_alpha,
			callback=self.set_clock_alpha,
			minimum=0.0,
			maximum=0.5,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_clock_alpha_sizer, 0, 3, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.agc = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)
		self.decoder = noaa.hrpt_decoder(True,True)
		self.deframer = noaa.hrpt_deframer()
		self.demod_scope = scopesink2.scope_sink_f(
			self.displays.GetPage(1).GetWin(),
			title="Post-Demod",
			sample_rate=sym_rate*2.0,
			v_scale=0.5,
			v_offset=0,
			t_scale=10.0/sym_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(1).GridAdd(self.demod_scope.win, 0, 0, 1, 1)
		self.frame_sink = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.gr_binary_slicer_fb_0 = gr.binary_slicer_fb()
		self.gr_clock_recovery_mm_xx_0 = gr.clock_recovery_mm_ff(sps/2.0, clock_alpha**2/4.0, 0.5, clock_alpha, max_clock_offset)
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.rx_fft = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=freq,
			y_per_div=5,
			y_divs=8,
			ref_level=-5,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=15,
			average=True,
			avg_alpha=0.1,
			title="RX Spectrum",
			peak_hold=False,
			size=(640, 360),
		)
		self.displays.GetPage(0).GridAdd(self.rx_fft.win, 0, 0, 1, 1)
		self.usrp_source = grc_usrp.simple_source_c(which=0, side=side, rx_ant="RXA")
		self.usrp_source.set_decim_rate(decim)
		self.usrp_source.set_frequency(freq, verbose=True)
		self.usrp_source.set_gain(gain)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_clock_recovery_mm_xx_0, 0), (self.gr_binary_slicer_fb_0, 0))
		self.connect((self.deframer, 0), (self.frame_sink, 0))
		self.connect((self.deframer, 0), (self.decoder, 0))
		self.connect((self.gr_clock_recovery_mm_xx_0, 0), (self.demod_scope, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.gr_clock_recovery_mm_xx_0, 0))
		self.connect((self.pll, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.agc, 0), (self.pll, 0))
		self.connect((self.usrp_source, 0), (self.agc, 0))
		self.connect((self.agc, 0), (self.rx_fft, 0))
		self.connect((self.gr_binary_slicer_fb_0, 0), (self.deframer, 0))

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section('usrp_rx_hrpt'):
			self._saved_freq_config.add_section('usrp_rx_hrpt')
		self._saved_freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section('usrp_rx_hrpt'):
			self._saved_gain_config.add_section('usrp_rx_hrpt')
		self._saved_gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_hrpt'):
			self._side_config.add_section('usrp_rx_hrpt')
		self._side_config.set('usrp_rx_hrpt', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(self.config_filename)
		if not self._decim_config.has_section('usrp_rx_hrpt'):
			self._decim_config.add_section('usrp_rx_hrpt')
		self._decim_config.set('usrp_rx_hrpt', 'decim', str(self.decim))
		self._decim_config.write(open(self.config_filename, 'w'))
		self._saved_pll_alpha_config = ConfigParser.ConfigParser()
		self._saved_pll_alpha_config.read(self.config_filename)
		if not self._saved_pll_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_pll_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_pll_alpha_config.set('usrp_rx_hrpt', 'pll_alpha', str(self.pll_alpha))
		self._saved_pll_alpha_config.write(open(self.config_filename, 'w'))
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(self.config_filename)
		if not self._saved_clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_clock_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._saved_clock_alpha_config.write(open(self.config_filename, 'w'))
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('usrp_rx_hrpt'):
			self._output_filename_config.add_section('usrp_rx_hrpt')
		self._output_filename_config.set('usrp_rx_hrpt', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)
		self.set_decim_text(self.decim)
		self.usrp_source.set_decim_rate(self.decim)
		self._decim_config = ConfigParser.ConfigParser()
		self._decim_config.read(self.config_filename)
		if not self._decim_config.has_section('usrp_rx_hrpt'):
			self._decim_config.add_section('usrp_rx_hrpt')
		self._decim_config.set('usrp_rx_hrpt', 'decim', str(self.decim))
		self._decim_config.write(open(self.config_filename, 'w'))

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)
		self.demod_scope.set_sample_rate(self.sym_rate*2.0)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.set_sps(self.sample_rate/self.sym_rate)
		self.rx_fft.set_sample_rate(self.sample_rate)

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))
		self.gr_clock_recovery_mm_xx_0.set_omega(self.sps/2.0)

	def set_side(self, side):
		self.side = side
		self.set_side_text(self.side)
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_hrpt'):
			self._side_config.add_section('usrp_rx_hrpt')
		self._side_config.set('usrp_rx_hrpt', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))

	def set_saved_pll_alpha(self, saved_pll_alpha):
		self.saved_pll_alpha = saved_pll_alpha
		self.set_pll_alpha(self.saved_pll_alpha)

	def set_saved_gain(self, saved_gain):
		self.saved_gain = saved_gain
		self.set_gain(self.saved_gain)

	def set_saved_freq(self, saved_freq):
		self.saved_freq = saved_freq
		self.set_freq(self.saved_freq)

	def set_saved_clock_alpha(self, saved_clock_alpha):
		self.saved_clock_alpha = saved_clock_alpha
		self.set_clock_alpha(self.saved_clock_alpha)

	def set_side_text(self, side_text):
		self.side_text = side_text
		self._side_text_static_text.set_value(self.side_text)

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self._pll_alpha_slider.set_value(self.pll_alpha)
		self._pll_alpha_text_box.set_value(self.pll_alpha)
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)
		self._saved_pll_alpha_config = ConfigParser.ConfigParser()
		self._saved_pll_alpha_config.read(self.config_filename)
		if not self._saved_pll_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_pll_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_pll_alpha_config.set('usrp_rx_hrpt', 'pll_alpha', str(self.pll_alpha))
		self._saved_pll_alpha_config.write(open(self.config_filename, 'w'))

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
		self._gain_slider.set_value(self.gain)
		self._gain_text_box.set_value(self.gain)
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section('usrp_rx_hrpt'):
			self._saved_gain_config.add_section('usrp_rx_hrpt')
		self._saved_gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_gain(self.gain)

	def set_freq(self, freq):
		self.freq = freq
		self._freq_text_box.set_value(self.freq)
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section('usrp_rx_hrpt'):
			self._saved_freq_config.add_section('usrp_rx_hrpt')
		self._saved_freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))
		self.usrp_source.set_frequency(self.freq)
		self.rx_fft.set_baseband_freq(self.freq)

	def set_decim_text(self, decim_text):
		self.decim_text = decim_text
		self._decim_text_static_text.set_value(self.decim_text)

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self._clock_alpha_slider.set_value(self.clock_alpha)
		self._clock_alpha_text_box.set_value(self.clock_alpha)
		self.gr_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.gr_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(self.config_filename)
		if not self._saved_clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_clock_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._saved_clock_alpha_config.write(open(self.config_filename, 'w'))

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_hrpt()
	tb.Run(True)

