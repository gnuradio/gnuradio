#
# Copyright 2008 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

##################################################
# Imports
##################################################
import plotter
import common
import wx
import numpy
import math
import pubsub
from constants import *
from gnuradio import gr #for gr.prefs

##################################################
# Constants
##################################################
SLIDER_STEPS = 100
AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP = -3, 0
DEFAULT_WIN_SIZE = (600, 300)
DEFAULT_FRAME_RATE = gr.prefs().get_long('wxgui', 'fft_rate', 30)
DIV_LEVELS = (1, 2, 5, 10, 20)
FFT_PLOT_COLOR_SPEC = (0.3, 0.3, 1.0)
PEAK_VALS_COLOR_SPEC = (0.0, 0.8, 0.0)
NO_PEAK_VALS = list()

##################################################
# FFT window control panel
##################################################
class control_panel(wx.Panel):
	"""
	A control panel with wx widgits to control the plotter and fft block chain.
	"""

	def __init__(self, parent):
		"""
		Create a new control panel.
		@param parent the wx parent window
		"""
		self.parent = parent
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		control_box = wx.BoxSizer(wx.VERTICAL)
		#checkboxes for average and peak hold
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Options'), 0, wx.ALIGN_CENTER)
		peak_hold_check_box = common.CheckBoxController(self, 'Peak Hold', parent, PEAK_HOLD_KEY)
		control_box.Add(peak_hold_check_box, 0, wx.EXPAND)
		average_check_box = common.CheckBoxController(self, 'Average', parent, AVERAGE_KEY)
		control_box.Add(average_check_box, 0, wx.EXPAND)
		control_box.AddSpacer(2)
		avg_alpha_slider = common.LogSliderController(
			self, 'Avg Alpha',
			AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP, SLIDER_STEPS,
			parent, AVG_ALPHA_KEY,
			formatter=lambda x: ': %.4f'%x,
		)
		parent.subscribe(AVERAGE_KEY, avg_alpha_slider.Enable)
		control_box.Add(avg_alpha_slider, 0, wx.EXPAND)
		#radio buttons for div size
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Set dB/div'), 0, wx.ALIGN_CENTER)
		radio_box = wx.BoxSizer(wx.VERTICAL)
		self.radio_buttons = list()
		for y_per_div in DIV_LEVELS:
			radio_button = wx.RadioButton(self, label="%d dB/div"%y_per_div)
			radio_button.Bind(wx.EVT_RADIOBUTTON, self._on_y_per_div)
			self.radio_buttons.append(radio_button)
			radio_box.Add(radio_button, 0, wx.ALIGN_LEFT)
		parent.subscribe(Y_PER_DIV_KEY, self._on_set_y_per_div)
		control_box.Add(radio_box, 0, wx.EXPAND)
		#ref lvl buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Set Ref Level'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		_ref_lvl_buttons = common.IncrDecrButtons(self, self._on_incr_ref_level, self._on_decr_ref_level)
		control_box.Add(_ref_lvl_buttons, 0, wx.ALIGN_CENTER)
		#autoscale
		control_box.AddStretchSpacer()
		autoscale_button = wx.Button(self, label='Autoscale', style=wx.BU_EXACTFIT)
		autoscale_button.Bind(wx.EVT_BUTTON, self.parent.autoscale)
		control_box.Add(autoscale_button, 0, wx.EXPAND)
		#run/stop
		run_button = common.ToggleButtonController(self, parent, RUNNING_KEY, 'Stop', 'Run')
		control_box.Add(run_button, 0, wx.EXPAND)
		#set sizer
		self.SetSizerAndFit(control_box)
		#mouse wheel event
		def on_mouse_wheel(event):
			if event.GetWheelRotation() < 0: self._on_incr_ref_level(event)
			else: self._on_decr_ref_level(event)
		parent.plotter.Bind(wx.EVT_MOUSEWHEEL, on_mouse_wheel)

	##################################################
	# Event handlers
	##################################################
	def _on_set_y_per_div(self, y_per_div):
		try:
			index = list(DIV_LEVELS).index(y_per_div)
			self.radio_buttons[index].SetValue(True)
		except: pass
	def _on_y_per_div(self, event):
		selected_radio_button = filter(lambda rb: rb.GetValue(), self.radio_buttons)[0]
		index = self.radio_buttons.index(selected_radio_button)
		self.parent[Y_PER_DIV_KEY] = DIV_LEVELS[index]
	def _on_incr_ref_level(self, event):
		self.parent[REF_LEVEL_KEY] = self.parent[REF_LEVEL_KEY] + self.parent[Y_PER_DIV_KEY]
	def _on_decr_ref_level(self, event):
		self.parent[REF_LEVEL_KEY] = self.parent[REF_LEVEL_KEY] - self.parent[Y_PER_DIV_KEY]

##################################################
# FFT window with plotter and control panel
##################################################
class fft_window(wx.Panel, pubsub.pubsub):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		real,
		fft_size,
		baseband_freq,
		sample_rate_key,
		y_per_div,
		y_divs,
		ref_level,
		average_key,
		avg_alpha_key,
		peak_hold,
		msg_key,
	):
		pubsub.pubsub.__init__(self)
		#ensure y_per_div
		if y_per_div not in DIV_LEVELS: y_per_div = DIV_LEVELS[0]
		#setup
		self.samples = list()
		self.real = real
		self.fft_size = fft_size
		self._reset_peak_vals()
		#proxy the keys
		self.proxy(MSG_KEY, controller, msg_key)
		self.proxy(AVERAGE_KEY, controller, average_key)
		self.proxy(AVG_ALPHA_KEY, controller, avg_alpha_key)
		self.proxy(SAMPLE_RATE_KEY, controller, sample_rate_key)
		#init panel and plot
		wx.Panel.__init__(self, parent, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.channel_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.set_title(title)
		self.plotter.enable_legend(True)
		self.plotter.enable_point_label(True)
		self.plotter.enable_grid_lines(True)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#initialize values
		self[AVERAGE_KEY] = self[AVERAGE_KEY]
		self[AVG_ALPHA_KEY] = self[AVG_ALPHA_KEY]
		self[PEAK_HOLD_KEY] = peak_hold
		self[Y_PER_DIV_KEY] = y_per_div
		self[Y_DIVS_KEY] = y_divs
		self[X_DIVS_KEY] = 8 #approximate
		self[REF_LEVEL_KEY] = ref_level
		self[BASEBAND_FREQ_KEY] = baseband_freq
		self[RUNNING_KEY] = True
		#register events
		self.subscribe(AVERAGE_KEY, lambda x: self._reset_peak_vals())
		self.subscribe(MSG_KEY, self.handle_msg)
		self.subscribe(SAMPLE_RATE_KEY, self.update_grid)
		for key in (
			BASEBAND_FREQ_KEY,
			Y_PER_DIV_KEY, X_DIVS_KEY,
			Y_DIVS_KEY, REF_LEVEL_KEY,
		): self.subscribe(key, self.update_grid)
		#initial update
		self.update_grid()

	def autoscale(self, *args):
		"""
		Autoscale the fft plot to the last frame.
		Set the dynamic range and reference level.
		"""
		if not len(self.samples): return
		#get the peak level (max of the samples)
		peak_level = numpy.max(self.samples)
		#get the noise floor (averge the smallest samples)
		noise_floor = numpy.average(numpy.sort(self.samples)[:len(self.samples)/4])
		#padding
		noise_floor -= abs(noise_floor)*.5
		peak_level += abs(peak_level)*.1
		#set the reference level to a multiple of y divs
		self[REF_LEVEL_KEY] = self[Y_DIVS_KEY]*math.ceil(peak_level/self[Y_DIVS_KEY])
		#set the range to a clean number of the dynamic range
		self[Y_PER_DIV_KEY] = common.get_clean_num((peak_level - noise_floor)/self[Y_DIVS_KEY])

	def _reset_peak_vals(self): self.peak_vals = NO_PEAK_VALS

	def handle_msg(self, msg):
		"""
		Handle the message from the fft sink message queue.
		If complex, reorder the fft samples so the negative bins come first.
		If real, keep take only the positive bins.
		Plot the samples onto the grid as channel 1.
		If peak hold is enabled, plot peak vals as channel 2.
		@param msg the fft array as a character array
		"""
		if not self[RUNNING_KEY]: return
		#convert to floating point numbers
		samples = numpy.fromstring(msg, numpy.float32)[:self.fft_size] #only take first frame
		num_samps = len(samples)
		#reorder fft
		if self.real: samples = samples[:num_samps/2]
		else: samples = numpy.concatenate((samples[num_samps/2:], samples[:num_samps/2]))
		self.samples = samples
		#peak hold calculation
		if self[PEAK_HOLD_KEY]:
			if len(self.peak_vals) != len(samples): self.peak_vals = samples
			self.peak_vals = numpy.maximum(samples, self.peak_vals)
			#plot the peak hold
			self.plotter.set_waveform(
				channel='Peak',
				samples=self.peak_vals,
				color_spec=PEAK_VALS_COLOR_SPEC,
			)
		else:
			self._reset_peak_vals()
			self.plotter.clear_waveform(channel='Peak')
		#plot the fft
		self.plotter.set_waveform(
			channel='FFT',
			samples=samples,
			color_spec=FFT_PLOT_COLOR_SPEC,
		)
		#update the plotter
		self.plotter.update()

	def update_grid(self, *args):
		"""
		Update the plotter grid.
		This update method is dependent on the variables below.
		Determine the x and y axis grid parameters.
		The x axis depends on sample rate, baseband freq, and x divs.
		The y axis depends on y per div, y divs, and ref level.
		"""
		#grid parameters
		sample_rate = self[SAMPLE_RATE_KEY]
		baseband_freq = self[BASEBAND_FREQ_KEY]
		y_per_div = self[Y_PER_DIV_KEY]
		y_divs = self[Y_DIVS_KEY]
		x_divs = self[X_DIVS_KEY]
		ref_level = self[REF_LEVEL_KEY]
		#determine best fitting x_per_div
		if self.real: x_width = sample_rate/2.0
		else: x_width = sample_rate/1.0
		x_per_div = common.get_clean_num(x_width/x_divs)
		#update the x grid
		if self.real:
			self.plotter.set_x_grid(
				baseband_freq,
				baseband_freq + sample_rate/2.0,
				x_per_div, True,
			)
		else:
			self.plotter.set_x_grid(
				baseband_freq - sample_rate/2.0,
				baseband_freq + sample_rate/2.0,
				x_per_div, True,
			)
		#update x units
		self.plotter.set_x_label('Frequency', 'Hz')
		#update y grid
		self.plotter.set_y_grid(ref_level-y_per_div*y_divs, ref_level, y_per_div)
		#update y units
		self.plotter.set_y_label('Amplitude', 'dB')
		#update plotter
		self.plotter.update()
