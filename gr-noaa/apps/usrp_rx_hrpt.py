#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Thu Oct 27 13:26:42 2011
##################################################

from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio import uhd
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
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
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 4e6
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self.sps = sps = sample_rate/sym_rate
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
		self._addr_config = ConfigParser.ConfigParser()
		self._addr_config.read(config_filename)
		try: addr = self._addr_config.get('usrp_rx_hrpt', 'addr')
		except: addr = ""
		self.addr = addr
		self.rate_text = rate_text = sample_rate
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
		self.clock_alpha = clock_alpha = saved_clock_alpha
		self.addr_text = addr_text = addr

		##################################################
		# Blocks
		##################################################
		self._freq_text_box = forms.text_box(
			parent=self.GetWin(),
			value=self.freq,
			callback=self.set_freq,
			label="Frequency",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._freq_text_box, 0, 0, 1, 1)
		self.displays = self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Spectrum")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Demod")
		self.GridAdd(self.displays, 2, 0, 1, 4)
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
		self.uhd_usrp_source_0 = uhd.usrp_source(
			device_addr=addr,
			io_type=uhd.io_type.COMPLEX_FLOAT32,
			num_channels=1,
		)
		self.uhd_usrp_source_0.set_samp_rate(sample_rate)
		self.uhd_usrp_source_0.set_center_freq(freq, 0)
		self.uhd_usrp_source_0.set_gain(0, 0)
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
		self._rate_text_static_text = forms.static_text(
			parent=self.GetWin(),
			value=self.rate_text,
			callback=self.set_rate_text,
			label="Baseband Rate",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._rate_text_static_text, 1, 1, 1, 1)
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
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
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
		self.frame_sink = gr.file_sink(gr.sizeof_short*1, output_filename)
		self.frame_sink.set_unbuffered(False)
		self.digital_clock_recovery_mm_xx_0 = digital.clock_recovery_mm_ff(sps/2.0, clock_alpha**2/4.0, 0.5, clock_alpha, max_clock_offset)
		self.digital_binary_slicer_fb_0 = digital.binary_slicer_fb()
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
			trig_mode=gr.gr_TRIG_MODE_AUTO,
			y_axis_label="Counts",
		)
		self.displays.GetPage(1).GridAdd(self.demod_scope.win, 0, 0, 1, 1)
		self.deframer = noaa.hrpt_deframer()
		self.decoder = noaa.hrpt_decoder(True,True)
		self.agc = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)
		self._addr_text_static_text = forms.static_text(
			parent=self.GetWin(),
			value=self.addr_text,
			callback=self.set_addr_text,
			label="USRP Addr",
			converter=forms.str_converter(),
		)
		self.GridAdd(self._addr_text_static_text, 1, 0, 1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.deframer, 0), (self.frame_sink, 0))
		self.connect((self.deframer, 0), (self.decoder, 0))
		self.connect((self.agc, 0), (self.pll, 0))
		self.connect((self.agc, 0), (self.rx_fft, 0))
		self.connect((self.uhd_usrp_source_0, 0), (self.agc, 0))
		self.connect((self.pll, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.demod_scope, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.digital_clock_recovery_mm_xx_0, 0))
		self.connect((self.digital_clock_recovery_mm_xx_0, 0), (self.digital_binary_slicer_fb_0, 0))
		self.connect((self.digital_binary_slicer_fb_0, 0), (self.deframer, 0))

	def get_sym_rate(self):
		return self.sym_rate

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)
		self.demod_scope.set_sample_rate(self.sym_rate*2.0)

	def get_sample_rate(self):
		return self.sample_rate

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.set_sps(self.sample_rate/self.sym_rate)
		self.rx_fft.set_sample_rate(self.sample_rate)
		self.uhd_usrp_source_0.set_samp_rate(self.sample_rate)
		self.set_rate_text(self.sample_rate)

	def get_config_filename(self):
		return self.config_filename

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._addr_config = ConfigParser.ConfigParser()
		self._addr_config.read(self.config_filename)
		if not self._addr_config.has_section('usrp_rx_hrpt'):
			self._addr_config.add_section('usrp_rx_hrpt')
		self._addr_config.set('usrp_rx_hrpt', 'addr', str(self.addr))
		self._addr_config.write(open(self.config_filename, 'w'))
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(self.config_filename)
		if not self._output_filename_config.has_section('usrp_rx_hrpt'):
			self._output_filename_config.add_section('usrp_rx_hrpt')
		self._output_filename_config.set('usrp_rx_hrpt', 'filename', str(self.output_filename))
		self._output_filename_config.write(open(self.config_filename, 'w'))
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(self.config_filename)
		if not self._saved_clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_clock_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._saved_clock_alpha_config.write(open(self.config_filename, 'w'))
		self._saved_pll_alpha_config = ConfigParser.ConfigParser()
		self._saved_pll_alpha_config.read(self.config_filename)
		if not self._saved_pll_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_pll_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_pll_alpha_config.set('usrp_rx_hrpt', 'pll_alpha', str(self.pll_alpha))
		self._saved_pll_alpha_config.write(open(self.config_filename, 'w'))
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section('usrp_rx_hrpt'):
			self._saved_gain_config.add_section('usrp_rx_hrpt')
		self._saved_gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section('usrp_rx_hrpt'):
			self._saved_freq_config.add_section('usrp_rx_hrpt')
		self._saved_freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))

	def get_sps(self):
		return self.sps

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))
		self.digital_clock_recovery_mm_xx_0.set_omega(self.sps/2.0)

	def get_saved_pll_alpha(self):
		return self.saved_pll_alpha

	def set_saved_pll_alpha(self, saved_pll_alpha):
		self.saved_pll_alpha = saved_pll_alpha
		self.set_pll_alpha(self.saved_pll_alpha)

	def get_saved_gain(self):
		return self.saved_gain

	def set_saved_gain(self, saved_gain):
		self.saved_gain = saved_gain
		self.set_gain(self.saved_gain)

	def get_saved_freq(self):
		return self.saved_freq

	def set_saved_freq(self, saved_freq):
		self.saved_freq = saved_freq
		self.set_freq(self.saved_freq)

	def get_saved_clock_alpha(self):
		return self.saved_clock_alpha

	def set_saved_clock_alpha(self, saved_clock_alpha):
		self.saved_clock_alpha = saved_clock_alpha
		self.set_clock_alpha(self.saved_clock_alpha)

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
		self.set_addr_text(self.addr)

	def get_rate_text(self):
		return self.rate_text

	def set_rate_text(self, rate_text):
		self.rate_text = rate_text
		self._rate_text_static_text.set_value(self.rate_text)

	def get_pll_alpha(self):
		return self.pll_alpha

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
		self._gain_slider.set_value(self.gain)
		self._gain_text_box.set_value(self.gain)
		self._saved_gain_config = ConfigParser.ConfigParser()
		self._saved_gain_config.read(self.config_filename)
		if not self._saved_gain_config.has_section('usrp_rx_hrpt'):
			self._saved_gain_config.add_section('usrp_rx_hrpt')
		self._saved_gain_config.set('usrp_rx_hrpt', 'gain', str(self.gain))
		self._saved_gain_config.write(open(self.config_filename, 'w'))

	def get_freq(self):
		return self.freq

	def set_freq(self, freq):
		self.freq = freq
		self._freq_text_box.set_value(self.freq)
		self.rx_fft.set_baseband_freq(self.freq)
		self.uhd_usrp_source_0.set_center_freq(self.freq, 0)
		self._saved_freq_config = ConfigParser.ConfigParser()
		self._saved_freq_config.read(self.config_filename)
		if not self._saved_freq_config.has_section('usrp_rx_hrpt'):
			self._saved_freq_config.add_section('usrp_rx_hrpt')
		self._saved_freq_config.set('usrp_rx_hrpt', 'freq', str(self.freq))
		self._saved_freq_config.write(open(self.config_filename, 'w'))

	def get_clock_alpha(self):
		return self.clock_alpha

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self._clock_alpha_slider.set_value(self.clock_alpha)
		self._clock_alpha_text_box.set_value(self.clock_alpha)
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(self.config_filename)
		if not self._saved_clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_clock_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._saved_clock_alpha_config.write(open(self.config_filename, 'w'))
		self.digital_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.digital_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)

	def get_addr_text(self):
		return self.addr_text

	def set_addr_text(self, addr_text):
		self.addr_text = addr_text
		self._addr_text_static_text.set_value(self.addr_text)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_hrpt()
	tb.Run(True)

