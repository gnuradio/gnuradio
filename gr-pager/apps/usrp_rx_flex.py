#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP FLEX Pager Receiver (Single Channel)
# Generated: Thu Oct 29 11:03:16 2009
##################################################

from gnuradio import blks2
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import pager
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
import os, math
import wx

class usrp_rx_flex(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="USRP FLEX Pager Receiver (Single Channel)")

		##################################################
		# Variables
		##################################################
		self.config_filename = config_filename = os.environ["HOME"]+"/.gnuradio/config.conf"
		self.symbol_rate = symbol_rate = 3200
		self._saved_channel_config = ConfigParser.ConfigParser()
		self._saved_channel_config.read(config_filename)
		try: saved_channel = self._saved_channel_config.getint("gr-pager", "channel")
		except: saved_channel = 25
		self.saved_channel = saved_channel
		self._saved_band_freq_config = ConfigParser.ConfigParser()
		self._saved_band_freq_config.read(config_filename)
		try: saved_band_freq = self._saved_band_freq_config.getfloat("gr-pager", "band_center")
		except: saved_band_freq = 930.5125e6
		self.saved_band_freq = saved_band_freq
		self.deviation = deviation = 4800
		self.decim = decim = 20
		self.adc_rate = adc_rate = 64e6
		self.sample_rate = sample_rate = adc_rate/decim
		self.passband = passband = 2*(deviation+symbol_rate)
		self.channel_rate = channel_rate = 8*3200
		self.channel = channel = saved_channel
		self.band_freq = band_freq = saved_band_freq
		self._saved_rx_gain_config = ConfigParser.ConfigParser()
		self._saved_rx_gain_config.read(config_filename)
		try: saved_rx_gain = self._saved_rx_gain_config.getint("gr-pager", "rx_gain")
		except: saved_rx_gain = 40
		self.saved_rx_gain = saved_rx_gain
		self._saved_offset_config = ConfigParser.ConfigParser()
		self._saved_offset_config.read(config_filename)
		try: saved_offset = self._saved_offset_config.getfloat("gr-pager", "freq_offset")
		except: saved_offset = 0
		self.saved_offset = saved_offset
		self.freq = freq = band_freq+(channel-61)*25e3
		self.channel_taps = channel_taps = firdes.low_pass(10, sample_rate, passband/2.0, (channel_rate-passband)/2.0)
		self.rx_gain = rx_gain = saved_rx_gain
		self.offset = offset = saved_offset
		self.nchan_taps = nchan_taps = len(channel_taps)
		self.ma_ntaps = ma_ntaps = int(channel_rate/symbol_rate)
		self.freq_text = freq_text = freq
		self.demod_k = demod_k = 3*channel_rate/(2*math.pi*deviation)
		self.channel_decim = channel_decim = int(sample_rate/channel_rate)
		self.bb_interp = bb_interp = 5
		self.bb_decim = bb_decim = 8
		self.baseband_rate = baseband_rate = 16000

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "RX Spectrum")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Baseband")
		self.GridAdd(self.displays, 1, 0, 1, 5)

		##################################################
		# Controls
		##################################################
		_channel_sizer = wx.BoxSizer(wx.VERTICAL)
		self._channel_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_channel_sizer,
			value=self.channel,
			callback=self.set_channel,
			label="Channel",
			converter=forms.int_converter(),
			proportion=0,
		)
		self._channel_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_channel_sizer,
			value=self.channel,
			callback=self.set_channel,
			minimum=1,
			maximum=120,
			num_steps=119,
			style=wx.SL_HORIZONTAL,
			cast=int,
			proportion=1,
		)
		self.GridAdd(_channel_sizer, 0, 1, 1, 1)
		self._band_freq_text_box = forms.text_box(
			parent=self.GetWin(),
			value=self.band_freq,
			callback=self.set_band_freq,
			label="Band Freq.",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._band_freq_text_box, 0, 0, 1, 1)
		_rx_gain_sizer = wx.BoxSizer(wx.VERTICAL)
		self._rx_gain_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_rx_gain_sizer,
			value=self.rx_gain,
			callback=self.set_rx_gain,
			label="Analog Gain",
			converter=forms.int_converter(),
			proportion=0,
		)
		self._rx_gain_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_rx_gain_sizer,
			value=self.rx_gain,
			callback=self.set_rx_gain,
			minimum=0,
			maximum=100,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=int,
			proportion=1,
		)
		self.GridAdd(_rx_gain_sizer, 0, 4, 1, 1)
		_offset_sizer = wx.BoxSizer(wx.VERTICAL)
		self._offset_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_offset_sizer,
			value=self.offset,
			callback=self.set_offset,
			label="Freq. Offset",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._offset_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_offset_sizer,
			value=self.offset,
			callback=self.set_offset,
			minimum=-12.5e3,
			maximum=12.5e3,
			num_steps=100,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_offset_sizer, 0, 3, 1, 1)
		self._freq_text_static_text = forms.static_text(
			parent=self.GetWin(),
			value=self.freq_text,
			callback=self.set_freq_text,
			label="Ch. Freq",
			converter=forms.float_converter(),
		)
		self.GridAdd(self._freq_text_static_text, 0, 2, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.fm_demod = gr.quadrature_demod_cf(demod_k)
		self.gr_freq_xlating_fir_filter_xxx_0 = gr.freq_xlating_fir_filter_ccc(channel_decim, (channel_taps), band_freq-freq+offset, sample_rate)
		self.gr_null_sink_0 = gr.null_sink(gr.sizeof_int*1)
		self.gr_null_sink_0_0 = gr.null_sink(gr.sizeof_int*1)
		self.gr_null_sink_0_1 = gr.null_sink(gr.sizeof_int*1)
		self.gr_null_sink_0_2 = gr.null_sink(gr.sizeof_int*1)
		self.pager_flex_deinterleave_0 = pager.flex_deinterleave()
		self.pager_flex_deinterleave_0_0 = pager.flex_deinterleave()
		self.pager_flex_deinterleave_0_1 = pager.flex_deinterleave()
		self.pager_flex_deinterleave_0_1_0 = pager.flex_deinterleave()
		self.pager_flex_sync_0 = pager.flex_sync()
		self.pager_slicer_fb_0 = pager.slicer_fb(1e-6)
		self.resampler = blks2.rational_resampler_fff(
			interpolation=bb_interp,
			decimation=bb_decim,
			taps=([1.0/ma_ntaps,]*ma_ntaps*bb_interp),
			fractional_bw=None,
		)
		self.usrp_source = grc_usrp.simple_source_c(which=0, side="A", rx_ant="RXA")
		self.usrp_source.set_decim_rate(decim)
		self.usrp_source.set_frequency(band_freq, verbose=True)
		self.usrp_source.set_gain(rx_gain)
		self.wxgui_fftsink2_0 = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=band_freq,
			y_per_div=10,
			y_divs=10,
			ref_level=0,
			ref_scale=65536,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=30,
			average=False,
			avg_alpha=None,
			title="FLEX Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.wxgui_fftsink2_0.win, 0, 0, 1, 1)
		self.wxgui_fftsink2_1 = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=freq,
			y_per_div=10,
			y_divs=10,
			ref_level=0,
			ref_scale=65536,
			sample_rate=channel_rate,
			fft_size=1024,
			fft_rate=30,
			average=False,
			avg_alpha=None,
			title="Channel Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.wxgui_fftsink2_1.win, 1, 0, 1, 1)
		self.wxgui_scopesink2_0_0 = scopesink2.scope_sink_f(
			self.displays.GetPage(1).GetWin(),
			title="Baseband",
			sample_rate=16e3,
			v_scale=1,
			v_offset=0,
			t_scale=40.0/16e3,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(1).GridAdd(self.wxgui_scopesink2_0_0.win, 0, 0, 1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.gr_freq_xlating_fir_filter_xxx_0, 0), (self.wxgui_fftsink2_1, 0))
		self.connect((self.usrp_source, 0), (self.gr_freq_xlating_fir_filter_xxx_0, 0))
		self.connect((self.usrp_source, 0), (self.wxgui_fftsink2_0, 0))
		self.connect((self.gr_freq_xlating_fir_filter_xxx_0, 0), (self.fm_demod, 0))
		self.connect((self.resampler, 0), (self.wxgui_scopesink2_0_0, 0))
		self.connect((self.fm_demod, 0), (self.resampler, 0))
		self.connect((self.pager_slicer_fb_0, 0), (self.pager_flex_sync_0, 0))
		self.connect((self.resampler, 0), (self.pager_slicer_fb_0, 0))
		self.connect((self.pager_flex_sync_0, 1), (self.pager_flex_deinterleave_0_1_0, 0))
		self.connect((self.pager_flex_sync_0, 2), (self.pager_flex_deinterleave_0_1, 0))
		self.connect((self.pager_flex_sync_0, 0), (self.pager_flex_deinterleave_0, 0))
		self.connect((self.pager_flex_sync_0, 3), (self.pager_flex_deinterleave_0_0, 0))
		self.connect((self.pager_flex_deinterleave_0, 0), (self.gr_null_sink_0, 0))
		self.connect((self.pager_flex_deinterleave_0_1_0, 0), (self.gr_null_sink_0_0, 0))
		self.connect((self.pager_flex_deinterleave_0_1, 0), (self.gr_null_sink_0_1, 0))
		self.connect((self.pager_flex_deinterleave_0_0, 0), (self.gr_null_sink_0_2, 0))

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._saved_band_freq_config = ConfigParser.ConfigParser()
		self._saved_band_freq_config.read(self.config_filename)
		if not self._saved_band_freq_config.has_section("gr-pager"):
			self._saved_band_freq_config.add_section("gr-pager")
		self._saved_band_freq_config.set("gr-pager", "band_center", str(self.band_freq))
		self._saved_band_freq_config.write(open(self.config_filename, 'w'))
		self._saved_channel_config = ConfigParser.ConfigParser()
		self._saved_channel_config.read(self.config_filename)
		if not self._saved_channel_config.has_section("gr-pager"):
			self._saved_channel_config.add_section("gr-pager")
		self._saved_channel_config.set("gr-pager", "channel", str(self.channel))
		self._saved_channel_config.write(open(self.config_filename, 'w'))
		self._saved_offset_config = ConfigParser.ConfigParser()
		self._saved_offset_config.read(self.config_filename)
		if not self._saved_offset_config.has_section("gr-pager"):
			self._saved_offset_config.add_section("gr-pager")
		self._saved_offset_config.set("gr-pager", "freq_offset", str(self.offset))
		self._saved_offset_config.write(open(self.config_filename, 'w'))
		self._saved_rx_gain_config = ConfigParser.ConfigParser()
		self._saved_rx_gain_config.read(self.config_filename)
		if not self._saved_rx_gain_config.has_section("gr-pager"):
			self._saved_rx_gain_config.add_section("gr-pager")
		self._saved_rx_gain_config.set("gr-pager", "rx_gain", str(self.rx_gain))
		self._saved_rx_gain_config.write(open(self.config_filename, 'w'))

	def set_symbol_rate(self, symbol_rate):
		self.symbol_rate = symbol_rate
		self.set_passband(2*(self.deviation+self.symbol_rate))
		self.set_ma_ntaps(int(self.channel_rate/self.symbol_rate))

	def set_saved_channel(self, saved_channel):
		self.saved_channel = saved_channel
		self.set_channel(self.saved_channel)

	def set_saved_band_freq(self, saved_band_freq):
		self.saved_band_freq = saved_band_freq
		self.set_band_freq(self.saved_band_freq)

	def set_deviation(self, deviation):
		self.deviation = deviation
		self.set_demod_k(3*self.channel_rate/(2*math.pi*self.deviation))
		self.set_passband(2*(self.deviation+self.symbol_rate))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(self.adc_rate/self.decim)
		self.usrp_source.set_decim_rate(self.decim)

	def set_adc_rate(self, adc_rate):
		self.adc_rate = adc_rate
		self.set_sample_rate(self.adc_rate/self.decim)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.wxgui_fftsink2_0.set_sample_rate(self.sample_rate)
		self.set_channel_decim(int(self.sample_rate/self.channel_rate))
		self.set_channel_taps(firdes.low_pass(10, self.sample_rate, self.passband/2.0, (self.channel_rate-self.passband)/2.0))

	def set_passband(self, passband):
		self.passband = passband
		self.set_channel_taps(firdes.low_pass(10, self.sample_rate, self.passband/2.0, (self.channel_rate-self.passband)/2.0))

	def set_channel_rate(self, channel_rate):
		self.channel_rate = channel_rate
		self.wxgui_fftsink2_1.set_sample_rate(self.channel_rate)
		self.set_channel_decim(int(self.sample_rate/self.channel_rate))
		self.set_demod_k(3*self.channel_rate/(2*math.pi*self.deviation))
		self.set_channel_taps(firdes.low_pass(10, self.sample_rate, self.passband/2.0, (self.channel_rate-self.passband)/2.0))
		self.set_ma_ntaps(int(self.channel_rate/self.symbol_rate))

	def set_channel(self, channel):
		self.channel = channel
		self.set_freq(self.band_freq+(self.channel-61)*25e3)
		self._saved_channel_config = ConfigParser.ConfigParser()
		self._saved_channel_config.read(self.config_filename)
		if not self._saved_channel_config.has_section("gr-pager"):
			self._saved_channel_config.add_section("gr-pager")
		self._saved_channel_config.set("gr-pager", "channel", str(self.channel))
		self._saved_channel_config.write(open(self.config_filename, 'w'))
		self._channel_slider.set_value(self.channel)
		self._channel_text_box.set_value(self.channel)

	def set_band_freq(self, band_freq):
		self.band_freq = band_freq
		self.set_freq(self.band_freq+(self.channel-61)*25e3)
		self.wxgui_fftsink2_0.set_baseband_freq(self.band_freq)
		self.usrp_source.set_frequency(self.band_freq)
		self.gr_freq_xlating_fir_filter_xxx_0.set_center_freq(self.band_freq-self.freq+self.offset)
		self._saved_band_freq_config = ConfigParser.ConfigParser()
		self._saved_band_freq_config.read(self.config_filename)
		if not self._saved_band_freq_config.has_section("gr-pager"):
			self._saved_band_freq_config.add_section("gr-pager")
		self._saved_band_freq_config.set("gr-pager", "band_center", str(self.band_freq))
		self._saved_band_freq_config.write(open(self.config_filename, 'w'))
		self._band_freq_text_box.set_value(self.band_freq)

	def set_saved_rx_gain(self, saved_rx_gain):
		self.saved_rx_gain = saved_rx_gain
		self.set_rx_gain(self.saved_rx_gain)

	def set_saved_offset(self, saved_offset):
		self.saved_offset = saved_offset
		self.set_offset(self.saved_offset)

	def set_freq(self, freq):
		self.freq = freq
		self.gr_freq_xlating_fir_filter_xxx_0.set_center_freq(self.band_freq-self.freq+self.offset)
		self.wxgui_fftsink2_1.set_baseband_freq(self.freq)
		self.set_freq_text(self.freq)

	def set_channel_taps(self, channel_taps):
		self.channel_taps = channel_taps
		self.gr_freq_xlating_fir_filter_xxx_0.set_taps((self.channel_taps))
		self.set_nchan_taps(len(self.channel_taps))

	def set_rx_gain(self, rx_gain):
		self.rx_gain = rx_gain
		self.usrp_source.set_gain(self.rx_gain)
		self._saved_rx_gain_config = ConfigParser.ConfigParser()
		self._saved_rx_gain_config.read(self.config_filename)
		if not self._saved_rx_gain_config.has_section("gr-pager"):
			self._saved_rx_gain_config.add_section("gr-pager")
		self._saved_rx_gain_config.set("gr-pager", "rx_gain", str(self.rx_gain))
		self._saved_rx_gain_config.write(open(self.config_filename, 'w'))
		self._rx_gain_slider.set_value(self.rx_gain)
		self._rx_gain_text_box.set_value(self.rx_gain)

	def set_offset(self, offset):
		self.offset = offset
		self.gr_freq_xlating_fir_filter_xxx_0.set_center_freq(self.band_freq-self.freq+self.offset)
		self._saved_offset_config = ConfigParser.ConfigParser()
		self._saved_offset_config.read(self.config_filename)
		if not self._saved_offset_config.has_section("gr-pager"):
			self._saved_offset_config.add_section("gr-pager")
		self._saved_offset_config.set("gr-pager", "freq_offset", str(self.offset))
		self._saved_offset_config.write(open(self.config_filename, 'w'))
		self._offset_slider.set_value(self.offset)
		self._offset_text_box.set_value(self.offset)

	def set_nchan_taps(self, nchan_taps):
		self.nchan_taps = nchan_taps

	def set_ma_ntaps(self, ma_ntaps):
		self.ma_ntaps = ma_ntaps

	def set_freq_text(self, freq_text):
		self.freq_text = freq_text
		self._freq_text_static_text.set_value(self.freq_text)

	def set_demod_k(self, demod_k):
		self.demod_k = demod_k

	def set_channel_decim(self, channel_decim):
		self.channel_decim = channel_decim

	def set_bb_interp(self, bb_interp):
		self.bb_interp = bb_interp

	def set_bb_decim(self, bb_decim):
		self.bb_decim = bb_decim

	def set_baseband_rate(self, baseband_rate):
		self.baseband_rate = baseband_rate

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_flex()
	tb.Run(True)

