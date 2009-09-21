#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Sun Sep 20 19:09:57 2009
##################################################

from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.gr import firdes
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import math
import wx

class usrp_rx_hrpt(grc_wxgui.top_block_gui):

	def __init__(self, filename="", decim=16):
		grc_wxgui.top_block_gui.__init__(self, title="USRP HRPT Receiver")

		##################################################
		# Parameters
		##################################################
		self.filename = filename
		self.decim = decim

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self.sync_alpha = sync_alpha = 0.1
		self.pll_alpha = pll_alpha = 0.005
		self.max_sync_offset = max_sync_offset = 0.01
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "RX")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "PLL/SYNC")
		self.GridAdd(self.displays, 1, 0, 1, 2)

		##################################################
		# Controls
		##################################################
		_sync_alpha_sizer = wx.BoxSizer(wx.VERTICAL)
		self._sync_alpha_text_box = forms.text_box(
			parent=self.GetWin(),
			sizer=_sync_alpha_sizer,
			value=self.sync_alpha,
			callback=self.set_sync_alpha,
			label="SYNC Alpha",
			converter=forms.float_converter(),
			proportion=0,
		)
		self._sync_alpha_slider = forms.slider(
			parent=self.GetWin(),
			sizer=_sync_alpha_sizer,
			value=self.sync_alpha,
			callback=self.set_sync_alpha,
			minimum=0.0,
			maximum=0.5,
			num_steps=500,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_sync_alpha_sizer, 0, 1, 1, 1)
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
			num_steps=500,
			style=wx.SL_HORIZONTAL,
			cast=float,
			proportion=1,
		)
		self.GridAdd(_pll_alpha_sizer, 0, 0, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.agr = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)
		self.gr_deinterleave_0 = gr.deinterleave(gr.sizeof_float*1)
		self.gr_file_sink_1 = gr.file_sink(gr.sizeof_short*1, "frames.dat")
		self.gr_file_source_0 = gr.file_source(gr.sizeof_short*1, "filename", False)
		self.gr_float_to_complex_0 = gr.float_to_complex(1)
		self.gr_short_to_float_0 = gr.short_to_float()
		self.matched_filter = gr.moving_average_cc(hs, 1.0/hs, 4000)
		self.noaa_hrpt_deframer_0 = noaa.hrpt_deframer()
		self.noaa_hrpt_pll_cf_0 = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.noaa_hrpt_sync_fb_0 = noaa.hrpt_sync_fb(sync_alpha, sync_alpha**2/4.0, sps, max_sync_offset)
		self.rx_fftsink = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=1698e6,
			y_per_div=5,
			y_divs=8,
			ref_level=-5,
			ref_scale=2.0,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=30,
			average=True,
			avg_alpha=0.1,
			title="RX Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.rx_fftsink.win, 0, 0, 1, 1)
		self.throttle = gr.throttle(gr.sizeof_gr_complex*1, sample_rate)
		self.wxgui_scopesink2_0 = scopesink2.scope_sink_c(
			self.displays.GetPage(0).GetWin(),
			title="RX Waveform",
			sample_rate=sample_rate,
			v_scale=0,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(0).GridAdd(self.wxgui_scopesink2_0.win, 1, 0, 1, 1)
		self.wxgui_scopesink2_0_0 = scopesink2.scope_sink_f(
			self.displays.GetPage(1).GetWin(),
			title="Post-PLL",
			sample_rate=sample_rate,
			v_scale=0.5,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(1).GridAdd(self.wxgui_scopesink2_0_0.win, 0, 0, 1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.throttle, 0), (self.agr, 0))
		self.connect((self.noaa_hrpt_deframer_0, 0), (self.gr_file_sink_1, 0))
		self.connect((self.noaa_hrpt_sync_fb_0, 0), (self.noaa_hrpt_deframer_0, 0))
		self.connect((self.noaa_hrpt_pll_cf_0, 0), (self.noaa_hrpt_sync_fb_0, 0))
		self.connect((self.noaa_hrpt_pll_cf_0, 0), (self.wxgui_scopesink2_0_0, 0))
		self.connect((self.agr, 0), (self.wxgui_scopesink2_0, 0))
		self.connect((self.agr, 0), (self.rx_fftsink, 0))
		self.connect((self.agr, 0), (self.matched_filter, 0))
		self.connect((self.matched_filter, 0), (self.noaa_hrpt_pll_cf_0, 0))
		self.connect((self.gr_float_to_complex_0, 0), (self.throttle, 0))
		self.connect((self.gr_short_to_float_0, 0), (self.gr_deinterleave_0, 0))
		self.connect((self.gr_deinterleave_0, 1), (self.gr_float_to_complex_0, 1))
		self.connect((self.gr_deinterleave_0, 0), (self.gr_float_to_complex_0, 0))
		self.connect((self.gr_file_source_0, 0), (self.gr_short_to_float_0, 0))

	def set_filename(self, filename):
		self.filename = filename

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.wxgui_scopesink2_0_0.set_sample_rate(self.sample_rate)
		self.wxgui_scopesink2_0.set_sample_rate(self.sample_rate)
		self.rx_fftsink.set_sample_rate(self.sample_rate)
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.set_sps(self.sample_rate/self.sym_rate)

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))

	def set_sync_alpha(self, sync_alpha):
		self.sync_alpha = sync_alpha
		self.noaa_hrpt_sync_fb_0.set_alpha(self.sync_alpha)
		self.noaa_hrpt_sync_fb_0.set_beta(self.sync_alpha**2/4.0)
		self._sync_alpha_slider.set_value(self.sync_alpha)
		self._sync_alpha_text_box.set_value(self.sync_alpha)

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self._pll_alpha_slider.set_value(self.pll_alpha)
		self._pll_alpha_text_box.set_value(self.pll_alpha)
		self.noaa_hrpt_pll_cf_0.set_alpha(self.pll_alpha)
		self.noaa_hrpt_pll_cf_0.set_beta(self.pll_alpha**2/4.0)

	def set_max_sync_offset(self, max_sync_offset):
		self.max_sync_offset = max_sync_offset
		self.noaa_hrpt_sync_fb_0.set_max_offset(self.max_sync_offset)

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.noaa_hrpt_pll_cf_0.set_max_offset(self.max_carrier_offset)

	def set_hs(self, hs):
		self.hs = hs
		self.matched_filter.set_length_and_scale(self.hs, 1.0/self.hs)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-f", "--filename", dest="filename", type="string", default="",
		help="Set Filename [default=%default]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=16,
		help="Set Decimation [default=%default]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_hrpt(filename=options.filename, decim=options.decim)
	tb.Run(True)

