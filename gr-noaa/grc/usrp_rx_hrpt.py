#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Sat Aug 29 11:48:57 2009
##################################################

from gnuradio import gr
from gnuradio import noaa
from gnuradio.eng_option import eng_option
from gnuradio.wxgui import fftsink2
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import math
import wx

class usrp_rx_hrpt(grc_wxgui.top_block_gui):

	def __init__(self):
		grc_wxgui.top_block_gui.__init__(self, title="USRP HRPT Receiver")

		##################################################
		# Variables
		##################################################
		self.decim = decim = 16
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.sps = sps = sample_rate/sym_rate
		self.sync_alpha = sync_alpha = 0.005
		self.pll_alpha = pll_alpha = 0.005
		self.max_sync_offset = max_sync_offset = 0.01
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "RX")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "PLL")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "SYNC")
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
		self.gr_moving_average_xx_0 = gr.moving_average_cc(hs, 1.0/hs, 4000)
		self.noaa_hrpt_pll_cc_0 = noaa.hrpt_pll_cc(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.rx_fftsink = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=1698e6,
			y_per_div=5,
			y_divs=8,
			ref_level=20,
			sample_rate=sample_rate,
			fft_size=1024,
			fft_rate=30,
			average=True,
			avg_alpha=0.1,
			title="RX Spectrum",
			peak_hold=False,
		)
		self.displays.GetPage(0).GridAdd(self.rx_fftsink.win, 0, 0, 1, 1)
		self.src = gr.file_source(gr.sizeof_gr_complex*1, "poes-d16.dat", True)
		self.sync = noaa.hrpt_sync_cc(sync_alpha, sync_alpha**2/4.0, sps, max_sync_offset)
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
		self.wxgui_scopesink2_0_0 = scopesink2.scope_sink_c(
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
		self.wxgui_scopesink2_0_0_0 = scopesink2.scope_sink_c(
			self.displays.GetPage(1).GetWin(),
			title="Post-PLL",
			sample_rate=sample_rate,
			v_scale=0.5,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=True,
			num_inputs=1,
		)
		self.displays.GetPage(1).GridAdd(self.wxgui_scopesink2_0_0_0.win, 1, 0, 1, 1)
		self.wxgui_scopesink2_0_0_0_0 = scopesink2.scope_sink_c(
			self.displays.GetPage(2).GetWin(),
			title="Post-SYNC",
			sample_rate=sample_rate,
			v_scale=0.5,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=True,
			num_inputs=1,
		)
		self.displays.GetPage(2).GridAdd(self.wxgui_scopesink2_0_0_0_0.win, 1, 0, 1, 1)
		self.wxgui_scopesink2_0_0_1 = scopesink2.scope_sink_c(
			self.displays.GetPage(2).GetWin(),
			title="Post-SYNC",
			sample_rate=sample_rate,
			v_scale=0.5,
			t_scale=20.0/sample_rate,
			ac_couple=False,
			xy_mode=False,
			num_inputs=1,
		)
		self.displays.GetPage(2).GridAdd(self.wxgui_scopesink2_0_0_1.win, 0, 0, 1, 1)

		##################################################
		# Connections
		##################################################
		self.connect((self.src, 0), (self.throttle, 0))
		self.connect((self.throttle, 0), (self.agr, 0))
		self.connect((self.agr, 0), (self.gr_moving_average_xx_0, 0))
		self.connect((self.agr, 0), (self.rx_fftsink, 0))
		self.connect((self.agr, 0), (self.wxgui_scopesink2_0, 0))
		self.connect((self.gr_moving_average_xx_0, 0), (self.noaa_hrpt_pll_cc_0, 0))
		self.connect((self.noaa_hrpt_pll_cc_0, 0), (self.wxgui_scopesink2_0_0, 0))
		self.connect((self.noaa_hrpt_pll_cc_0, 0), (self.wxgui_scopesink2_0_0_0, 0))
		self.connect((self.noaa_hrpt_pll_cc_0, 0), (self.sync, 0))
		self.connect((self.sync, 0), (self.wxgui_scopesink2_0_0_1, 0))
		self.connect((self.sync, 0), (self.wxgui_scopesink2_0_0_0_0, 0))

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_sps(self.sample_rate/self.sym_rate)
		self.rx_fftsink.set_sample_rate(self.sample_rate)
		self.wxgui_scopesink2_0.set_sample_rate(self.sample_rate)
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.wxgui_scopesink2_0_0.set_sample_rate(self.sample_rate)
		self.wxgui_scopesink2_0_0_1.set_sample_rate(self.sample_rate)
		self.wxgui_scopesink2_0_0_0.set_sample_rate(self.sample_rate)
		self.wxgui_scopesink2_0_0_0_0.set_sample_rate(self.sample_rate)

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))

	def set_sync_alpha(self, sync_alpha):
		self.sync_alpha = sync_alpha
		self._sync_alpha_slider.set_value(self.sync_alpha)
		self._sync_alpha_text_box.set_value(self.sync_alpha)
		self.sync.set_alpha(self.sync_alpha)
		self.sync.set_beta(self.sync_alpha**2/4.0)

	def set_pll_alpha(self, pll_alpha):
		self.pll_alpha = pll_alpha
		self._pll_alpha_slider.set_value(self.pll_alpha)
		self._pll_alpha_text_box.set_value(self.pll_alpha)
		self.noaa_hrpt_pll_cc_0.set_alpha(self.pll_alpha)
		self.noaa_hrpt_pll_cc_0.set_beta(self.pll_alpha**2/4.0)

	def set_max_sync_offset(self, max_sync_offset):
		self.max_sync_offset = max_sync_offset
		self.sync.set_max_offset(self.max_sync_offset)

	def set_max_carrier_offset(self, max_carrier_offset):
		self.max_carrier_offset = max_carrier_offset
		self.noaa_hrpt_pll_cc_0.set_max_offset(self.max_carrier_offset)

	def set_hs(self, hs):
		self.hs = hs
		self.gr_moving_average_xx_0.set_length_and_scale(self.hs, 1.0/self.hs)

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	(options, args) = parser.parse_args()
	tb = usrp_rx_hrpt()
	tb.Run(True)

