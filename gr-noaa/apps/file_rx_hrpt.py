#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: USRP HRPT Receiver
# Generated: Mon Nov  2 08:30:47 2009
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
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import ConfigParser
import math, os
import wx

class file_rx_hrpt(grc_wxgui.top_block_gui):

	def __init__(self, input_filename='usrp.dat', decim=16):
		grc_wxgui.top_block_gui.__init__(self, title="USRP HRPT Receiver")

		##################################################
		# Parameters
		##################################################
		self.input_filename = input_filename
		self.decim = decim

		##################################################
		# Variables
		##################################################
		self.sym_rate = sym_rate = 600*1109
		self.sample_rate = sample_rate = 64e6/decim
		self.config_filename = config_filename = os.environ['HOME']+'/.gnuradio/config.conf'
		self.sps = sps = sample_rate/sym_rate
		self._saved_pll_alpha_config = ConfigParser.ConfigParser()
		self._saved_pll_alpha_config.read(config_filename)
		try: saved_pll_alpha = self._saved_pll_alpha_config.getfloat('usrp_rx_hrpt', 'pll_alpha')
		except: saved_pll_alpha = 0.05
		self.saved_pll_alpha = saved_pll_alpha
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(config_filename)
		try: saved_clock_alpha = self._saved_clock_alpha_config.getfloat('usrp_rx_hrpt', 'clock_alpha')
		except: saved_clock_alpha = 0.05
		self.saved_clock_alpha = saved_clock_alpha
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(config_filename)
		try: side = self._side_config.get('usrp_rx_hrpt', 'side')
		except: side = 'A'
		self.side = side
		self.pll_alpha = pll_alpha = saved_pll_alpha
		self._output_filename_config = ConfigParser.ConfigParser()
		self._output_filename_config.read(config_filename)
		try: output_filename = self._output_filename_config.get('usrp_rx_hrpt', 'filename')
		except: output_filename = 'frames.dat'
		self.output_filename = output_filename
		self.max_clock_offset = max_clock_offset = 0.1
		self.max_carrier_offset = max_carrier_offset = 2*math.pi*100e3/sample_rate
		self.hs = hs = int(sps/2.0)
		self.clock_alpha = clock_alpha = saved_clock_alpha

		##################################################
		# Notebooks
		##################################################
		self.displays = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Spectrum")
		self.displays.AddPage(grc_wxgui.Panel(self.displays), "Demod")
		self.GridAdd(self.displays, 1, 0, 1, 2)

		##################################################
		# Controls
		##################################################
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
		self.GridAdd(_pll_alpha_sizer, 0, 0, 1, 1)
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
		self.GridAdd(_clock_alpha_sizer, 0, 1, 1, 1)

		##################################################
		# Blocks
		##################################################
		self.agc = gr.agc_cc(1e-6, 1.0, 1.0, 1.0)
		self.decoder = noaa.hrpt_decoder()
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
		self.gr_file_source_0 = gr.file_source(gr.sizeof_short*1, input_filename, False)
		self.gr_interleaved_short_to_complex_0 = gr.interleaved_short_to_complex()
		self.gr_moving_average_xx_0 = gr.moving_average_ff(hs, 1.0/hs, 4000)
		self.pll = noaa.hrpt_pll_cf(pll_alpha, pll_alpha**2/4.0, max_carrier_offset)
		self.rx_fft = fftsink2.fft_sink_c(
			self.displays.GetPage(0).GetWin(),
			baseband_freq=0,
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
		self.throttle = gr.throttle(gr.sizeof_short*1, sample_rate)

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
		self.connect((self.agc, 0), (self.rx_fft, 0))
		self.connect((self.gr_interleaved_short_to_complex_0, 0), (self.agc, 0))
		self.connect((self.throttle, 0), (self.gr_interleaved_short_to_complex_0, 0))
		self.connect((self.gr_file_source_0, 0), (self.throttle, 0))
		self.connect((self.gr_binary_slicer_fb_0, 0), (self.deframer, 0))

	def set_input_filename(self, input_filename):
		self.input_filename = input_filename

	def set_decim(self, decim):
		self.decim = decim
		self.set_sample_rate(64e6/self.decim)

	def set_sym_rate(self, sym_rate):
		self.sym_rate = sym_rate
		self.set_sps(self.sample_rate/self.sym_rate)
		self.demod_scope.set_sample_rate(self.sym_rate*2.0)

	def set_sample_rate(self, sample_rate):
		self.sample_rate = sample_rate
		self.set_max_carrier_offset(2*math.pi*100e3/self.sample_rate)
		self.set_sps(self.sample_rate/self.sym_rate)
		self.rx_fft.set_sample_rate(self.sample_rate)

	def set_config_filename(self, config_filename):
		self.config_filename = config_filename
		self._side_config = ConfigParser.ConfigParser()
		self._side_config.read(self.config_filename)
		if not self._side_config.has_section('usrp_rx_hrpt'):
			self._side_config.add_section('usrp_rx_hrpt')
		self._side_config.set('usrp_rx_hrpt', 'side', str(self.side))
		self._side_config.write(open(self.config_filename, 'w'))
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

	def set_sps(self, sps):
		self.sps = sps
		self.set_hs(int(self.sps/2.0))
		self.gr_clock_recovery_mm_xx_0.set_omega(self.sps/2.0)

	def set_saved_pll_alpha(self, saved_pll_alpha):
		self.saved_pll_alpha = saved_pll_alpha
		self.set_pll_alpha(self.saved_pll_alpha)

	def set_saved_clock_alpha(self, saved_clock_alpha):
		self.saved_clock_alpha = saved_clock_alpha
		self.set_clock_alpha(self.saved_clock_alpha)

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
		self.pll.set_alpha(self.pll_alpha)
		self.pll.set_beta(self.pll_alpha**2/4.0)
		self._pll_alpha_slider.set_value(self.pll_alpha)
		self._pll_alpha_text_box.set_value(self.pll_alpha)
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

	def set_clock_alpha(self, clock_alpha):
		self.clock_alpha = clock_alpha
		self.gr_clock_recovery_mm_xx_0.set_gain_omega(self.clock_alpha**2/4.0)
		self.gr_clock_recovery_mm_xx_0.set_gain_mu(self.clock_alpha)
		self._clock_alpha_slider.set_value(self.clock_alpha)
		self._clock_alpha_text_box.set_value(self.clock_alpha)
		self._saved_clock_alpha_config = ConfigParser.ConfigParser()
		self._saved_clock_alpha_config.read(self.config_filename)
		if not self._saved_clock_alpha_config.has_section('usrp_rx_hrpt'):
			self._saved_clock_alpha_config.add_section('usrp_rx_hrpt')
		self._saved_clock_alpha_config.set('usrp_rx_hrpt', 'clock_alpha', str(self.clock_alpha))
		self._saved_clock_alpha_config.write(open(self.config_filename, 'w'))

if __name__ == '__main__':
	parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
	parser.add_option("-F", "--input-filename", dest="input_filename", type="string", default='usrp.dat',
		help="Set usrp.dat [default=%default]")
	parser.add_option("-d", "--decim", dest="decim", type="intx", default=16,
		help="Set decim [default=%default]")
	(options, args) = parser.parse_args()
	tb = file_rx_hrpt(input_filename=options.input_filename, decim=options.decim)
	tb.Run(True)

