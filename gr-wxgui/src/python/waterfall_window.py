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
# Boston, MA 02110-1`301, USA.
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
DEFAULT_FRAME_RATE = gr.prefs().get_long('wxgui', 'waterfall_rate', 30)
DEFAULT_WIN_SIZE = (600, 300)
DIV_LEVELS = (1, 2, 5, 10, 20)
MIN_DYNAMIC_RANGE, MAX_DYNAMIC_RANGE = 10, 200
COLOR_MODES = (
	('RGB1', 'rgb1'),
	('RGB2', 'rgb2'),
	('RGB3', 'rgb3'),
	('Gray', 'gray'),
)

##################################################
# Waterfall window control panel
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
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Options'), 0, wx.ALIGN_CENTER)
		#color mode
		control_box.AddStretchSpacer()
		color_mode_chooser = common.DropDownController(self, COLOR_MODES, parent, COLOR_MODE_KEY)
		control_box.Add(common.LabelBox(self, 'Color', color_mode_chooser), 0, wx.EXPAND)
		#average
		control_box.AddStretchSpacer()
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
		#dyanmic range buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Dynamic Range'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		dynamic_range_buttons = common.IncrDecrButtons(self, self._on_incr_dynamic_range, self._on_decr_dynamic_range)
		control_box.Add(dynamic_range_buttons, 0, wx.ALIGN_CENTER)
		#ref lvl buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Set Ref Level'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		ref_lvl_buttons = common.IncrDecrButtons(self, self._on_incr_ref_level, self._on_decr_ref_level)
		control_box.Add(ref_lvl_buttons, 0, wx.ALIGN_CENTER)
		#num lines buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Set Time Scale'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		time_scale_buttons = common.IncrDecrButtons(self, self._on_incr_time_scale, self._on_decr_time_scale)
		control_box.Add(time_scale_buttons, 0, wx.ALIGN_CENTER)
		#autoscale
		control_box.AddStretchSpacer()
		autoscale_button = wx.Button(self, label='Autoscale', style=wx.BU_EXACTFIT)
		autoscale_button.Bind(wx.EVT_BUTTON, self.parent.autoscale)
		control_box.Add(autoscale_button, 0, wx.EXPAND)
		#clear
		clear_button = wx.Button(self, label='Clear', style=wx.BU_EXACTFIT)
		clear_button.Bind(wx.EVT_BUTTON, self._on_clear_button)
		control_box.Add(clear_button, 0, wx.EXPAND)
		#run/stop
		run_button = common.ToggleButtonController(self, parent, RUNNING_KEY, 'Stop', 'Run')
		control_box.Add(run_button, 0, wx.EXPAND)
		#set sizer
		self.SetSizerAndFit(control_box)

	##################################################
	# Event handlers
	##################################################
	def _on_clear_button(self, event):
		self.parent[NUM_LINES_KEY] = self.parent[NUM_LINES_KEY]
	def _on_incr_dynamic_range(self, event):
		self.parent[DYNAMIC_RANGE_KEY] = min(self.parent[DYNAMIC_RANGE_KEY] + 10, MAX_DYNAMIC_RANGE)
	def _on_decr_dynamic_range(self, event):
		self.parent[DYNAMIC_RANGE_KEY] = max(self.parent[DYNAMIC_RANGE_KEY] - 10, MIN_DYNAMIC_RANGE)
	def _on_incr_ref_level(self, event):
		self.parent[REF_LEVEL_KEY] = self.parent[REF_LEVEL_KEY] + self.parent[DYNAMIC_RANGE_KEY]*.1
	def _on_decr_ref_level(self, event):
		self.parent[REF_LEVEL_KEY] = self.parent[REF_LEVEL_KEY] - self.parent[DYNAMIC_RANGE_KEY]*.1
	def _on_incr_time_scale(self, event):
		old_rate = self.parent[FRAME_RATE_KEY]
		self.parent[FRAME_RATE_KEY] *= 0.75
		if self.parent[FRAME_RATE_KEY] == old_rate:
			self.parent[DECIMATION_KEY] += 1
	def _on_decr_time_scale(self, event):
		old_rate = self.parent[FRAME_RATE_KEY]
		self.parent[FRAME_RATE_KEY] *= 1.25
		if self.parent[FRAME_RATE_KEY] == old_rate:
			self.parent[DECIMATION_KEY] -= 1

##################################################
# Waterfall window with plotter and control panel
##################################################
class waterfall_window(wx.Panel, pubsub.pubsub):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		real,
		fft_size,
		num_lines,
		decimation_key,
		baseband_freq,
		sample_rate_key,
		frame_rate_key,
		dynamic_range,
		ref_level,
		average_key,
		avg_alpha_key,
		msg_key,
	):
		pubsub.pubsub.__init__(self)
		#setup
		self.samples = list()
		self.real = real
		self.fft_size = fft_size
		#proxy the keys
		self.proxy(MSG_KEY, controller, msg_key)
		self.proxy(DECIMATION_KEY, controller, decimation_key)
		self.proxy(FRAME_RATE_KEY, controller, frame_rate_key)
		self.proxy(AVERAGE_KEY, controller, average_key)
		self.proxy(AVG_ALPHA_KEY, controller, avg_alpha_key)
		self.proxy(SAMPLE_RATE_KEY, controller, sample_rate_key)
		#init panel and plot
		wx.Panel.__init__(self, parent, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.waterfall_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.set_title(title)
		self.plotter.enable_point_label(True)
		self.plotter.enable_grid_lines(False)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#plotter listeners
		self.subscribe(COLOR_MODE_KEY, self.plotter.set_color_mode)
		self.subscribe(NUM_LINES_KEY, self.plotter.set_num_lines)
		#initialize values
		self[AVERAGE_KEY] = self[AVERAGE_KEY]
		self[AVG_ALPHA_KEY] = self[AVG_ALPHA_KEY]
		self[DYNAMIC_RANGE_KEY] = dynamic_range
		self[NUM_LINES_KEY] = num_lines
		self[Y_DIVS_KEY] = 8
		self[X_DIVS_KEY] = 8 #approximate
		self[REF_LEVEL_KEY] = ref_level
		self[BASEBAND_FREQ_KEY] = baseband_freq
		self[COLOR_MODE_KEY] = COLOR_MODES[0][1]
		self[RUNNING_KEY] = True
		#register events
		self.subscribe(MSG_KEY, self.handle_msg)
		for key in (
			DECIMATION_KEY, SAMPLE_RATE_KEY, FRAME_RATE_KEY,
			BASEBAND_FREQ_KEY, X_DIVS_KEY, Y_DIVS_KEY, NUM_LINES_KEY,
		): self.subscribe(key, self.update_grid)
		#initial update
		self.update_grid()

	def autoscale(self, *args):
		"""
		Autoscale the waterfall plot to the last frame.
		Set the dynamic range and reference level.
		Does not affect the current data in the waterfall.
		"""
		if not len(self.samples): return
		#get the peak level (max of the samples)
		peak_level = numpy.max(self.samples)
		#get the noise floor (averge the smallest samples)
		noise_floor = numpy.average(numpy.sort(self.samples)[:len(self.samples)/4])
		#padding
		noise_floor -= abs(noise_floor)*.5
		peak_level += abs(peak_level)*.1
		#set the range and level
		self[REF_LEVEL_KEY] = peak_level
		self[DYNAMIC_RANGE_KEY] = peak_level - noise_floor

	def handle_msg(self, msg):
		"""
		Handle the message from the fft sink message queue.
		If complex, reorder the fft samples so the negative bins come first.
		If real, keep take only the positive bins.
		Send the data to the plotter.
		@param msg the fft array as a character array
		"""
		if not self[RUNNING_KEY]: return
		#convert to floating point numbers
		self.samples = samples = numpy.fromstring(msg, numpy.float32)[:self.fft_size] #only take first frame
		num_samps = len(samples)
		#reorder fft
		if self.real: samples = samples[:num_samps/2]
		else: samples = numpy.concatenate((samples[num_samps/2:], samples[:num_samps/2]))
		#plot the fft
		self.plotter.set_samples(
			samples=samples,
			minimum=self[REF_LEVEL_KEY] - self[DYNAMIC_RANGE_KEY], 
			maximum=self[REF_LEVEL_KEY],
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
		frame_rate = self[FRAME_RATE_KEY]
		baseband_freq = self[BASEBAND_FREQ_KEY]
		num_lines = self[NUM_LINES_KEY]
		y_divs = self[Y_DIVS_KEY]
		x_divs = self[X_DIVS_KEY]
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
		duration = float(num_lines)/frame_rate
		y_per_div = common.get_clean_num(duration/y_divs)
		self.plotter.set_y_grid(0, duration, y_per_div, True)
		#update y units
		self.plotter.set_y_label('Time', 's')
		#update plotter
		self.plotter.update()
