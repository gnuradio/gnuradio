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

##################################################
# Constants
##################################################
SLIDER_STEPS = 100
AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP = -3, 0
DEFAULT_FRAME_RATE = 30
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
		wx.Panel.__init__(self, parent, -1, style=wx.SUNKEN_BORDER)
		control_box = wx.BoxSizer(wx.VERTICAL)
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Options'), 0, wx.ALIGN_CENTER)
		#color mode
		control_box.AddStretchSpacer()
		self.color_mode_chooser = common.DropDownController(self, 'Color', COLOR_MODES, parent, COLOR_MODE_KEY)
		control_box.Add(self.color_mode_chooser, 0, wx.EXPAND)
		#average
		control_box.AddStretchSpacer()
		self.average_check_box = common.CheckBoxController(self, 'Average', parent.ext_controller, parent.average_key)
		control_box.Add(self.average_check_box, 0, wx.EXPAND)
		control_box.AddSpacer(2)
		self.avg_alpha_slider = common.LogSliderController(
			self, 'Avg Alpha',
			AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP, SLIDER_STEPS,
			parent.ext_controller, parent.avg_alpha_key,
			formatter=lambda x: ': %.4f'%x,
		)
		parent.ext_controller.subscribe(parent.average_key, self.avg_alpha_slider.Enable)
		control_box.Add(self.avg_alpha_slider, 0, wx.EXPAND)
		#dyanmic range buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Dynamic Range'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		self._dynamic_range_buttons = common.IncrDecrButtons(self, self._on_incr_dynamic_range, self._on_decr_dynamic_range)
		control_box.Add(self._dynamic_range_buttons, 0, wx.ALIGN_CENTER)
		#ref lvl buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Set Ref Level'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		self._ref_lvl_buttons = common.IncrDecrButtons(self, self._on_incr_ref_level, self._on_decr_ref_level)
		control_box.Add(self._ref_lvl_buttons, 0, wx.ALIGN_CENTER)
		#num lines buttons
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Set Time Scale'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		self._time_scale_buttons = common.IncrDecrButtons(self, self._on_incr_time_scale, self._on_decr_time_scale)
		control_box.Add(self._time_scale_buttons, 0, wx.ALIGN_CENTER)
		#autoscale
		control_box.AddStretchSpacer()
		self.autoscale_button = wx.Button(self, label='Autoscale', style=wx.BU_EXACTFIT)
		self.autoscale_button.Bind(wx.EVT_BUTTON, self.parent.autoscale)
		control_box.Add(self.autoscale_button, 0, wx.EXPAND)
		#clear
		self.clear_button = wx.Button(self, label='Clear', style=wx.BU_EXACTFIT)
		self.clear_button.Bind(wx.EVT_BUTTON, self._on_clear_button)
		control_box.Add(self.clear_button, 0, wx.EXPAND)
		#run/stop
		self.run_button = common.ToggleButtonController(self, parent, RUNNING_KEY, 'Stop', 'Run')
		control_box.Add(self.run_button, 0, wx.EXPAND)
		#set sizer
		self.SetSizerAndFit(control_box)

	##################################################
	# Event handlers
	##################################################
	def _on_clear_button(self, event):
		self.parent.set_num_lines(self.parent[NUM_LINES_KEY])
	def _on_incr_dynamic_range(self, event):
		self.parent.set_dynamic_range(
			min(self.parent[DYNAMIC_RANGE_KEY] + 10, MAX_DYNAMIC_RANGE))
	def _on_decr_dynamic_range(self, event):
		self.parent.set_dynamic_range(
			max(self.parent[DYNAMIC_RANGE_KEY] - 10, MIN_DYNAMIC_RANGE))
	def _on_incr_ref_level(self, event):
		self.parent.set_ref_level(
			self.parent[REF_LEVEL_KEY] + self.parent[DYNAMIC_RANGE_KEY]*.1)
	def _on_decr_ref_level(self, event):
		self.parent.set_ref_level(
			self.parent[REF_LEVEL_KEY] - self.parent[DYNAMIC_RANGE_KEY]*.1)
	def _on_incr_time_scale(self, event):
		old_rate = self.parent.ext_controller[self.parent.frame_rate_key]
		self.parent.ext_controller[self.parent.frame_rate_key] *= 0.75
		if self.parent.ext_controller[self.parent.frame_rate_key] == old_rate:
			self.parent.ext_controller[self.parent.decimation_key] += 1
	def _on_decr_time_scale(self, event):
		old_rate = self.parent.ext_controller[self.parent.frame_rate_key]
		self.parent.ext_controller[self.parent.frame_rate_key] *= 1.25
		if self.parent.ext_controller[self.parent.frame_rate_key] == old_rate:
			self.parent.ext_controller[self.parent.decimation_key] -= 1

##################################################
# Waterfall window with plotter and control panel
##################################################
class waterfall_window(wx.Panel, pubsub.pubsub, common.prop_setter):
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
		self.ext_controller = controller
		self.real = real
		self.fft_size = fft_size
		self.decimation_key = decimation_key
		self.sample_rate_key = sample_rate_key
		self.frame_rate_key = frame_rate_key
		self.average_key = average_key
		self.avg_alpha_key = avg_alpha_key
		#init panel and plot
		wx.Panel.__init__(self, parent, -1, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.waterfall_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.set_title(title)
		self.plotter.enable_point_label(True)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#plotter listeners
		self.subscribe(COLOR_MODE_KEY, self.plotter.set_color_mode)
		self.subscribe(NUM_LINES_KEY, self.plotter.set_num_lines)
		#initial setup
		self.ext_controller[self.average_key] = self.ext_controller[self.average_key]
		self.ext_controller[self.avg_alpha_key] = self.ext_controller[self.avg_alpha_key]
		self._register_set_prop(self, DYNAMIC_RANGE_KEY, dynamic_range)
		self._register_set_prop(self, NUM_LINES_KEY, num_lines)
		self._register_set_prop(self, Y_DIVS_KEY, 8)
		self._register_set_prop(self, X_DIVS_KEY, 8) #approximate
		self._register_set_prop(self, REF_LEVEL_KEY, ref_level)
		self._register_set_prop(self, BASEBAND_FREQ_KEY, baseband_freq)
		self._register_set_prop(self, COLOR_MODE_KEY, COLOR_MODES[0][1])
		self._register_set_prop(self, RUNNING_KEY, True)
		#register events
		self.ext_controller.subscribe(msg_key, self.handle_msg)
		self.ext_controller.subscribe(self.decimation_key, self.update_grid)
		self.ext_controller.subscribe(self.sample_rate_key, self.update_grid)
		self.ext_controller.subscribe(self.frame_rate_key, self.update_grid)
		self.subscribe(BASEBAND_FREQ_KEY, self.update_grid)
		self.subscribe(NUM_LINES_KEY, self.update_grid)
		self.subscribe(Y_DIVS_KEY, self.update_grid)
		self.subscribe(X_DIVS_KEY, self.update_grid)
		#initial update
		self.update_grid()

	def autoscale(self, *args):
		"""
		Autoscale the waterfall plot to the last frame.
		Set the dynamic range and reference level.
		Does not affect the current data in the waterfall.
		"""
		#get the peak level (max of the samples)
		peak_level = numpy.max(self.samples)
		#get the noise floor (averge the smallest samples)
		noise_floor = numpy.average(numpy.sort(self.samples)[:len(self.samples)/4])
		#padding
		noise_floor -= abs(noise_floor)*.5
		peak_level += abs(peak_level)*.1
		#set the range and level
		self.set_ref_level(peak_level)
		self.set_dynamic_range(peak_level - noise_floor)

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
		sample_rate = self.ext_controller[self.sample_rate_key]
		frame_rate = self.ext_controller[self.frame_rate_key]
		baseband_freq = self[BASEBAND_FREQ_KEY]
		num_lines = self[NUM_LINES_KEY]
		y_divs = self[Y_DIVS_KEY]
		x_divs = self[X_DIVS_KEY]
		#determine best fitting x_per_div
		if self.real: x_width = sample_rate/2.0
		else: x_width = sample_rate/1.0
		x_per_div = common.get_clean_num(x_width/x_divs)
		coeff, exp, prefix = common.get_si_components(abs(baseband_freq) + abs(sample_rate/2.0))
		#update the x grid
		if self.real:
			self.plotter.set_x_grid(
				baseband_freq,
				baseband_freq + sample_rate/2.0,
				x_per_div,
				10**(-exp),
			)
		else:
			self.plotter.set_x_grid(
				baseband_freq - sample_rate/2.0,
				baseband_freq + sample_rate/2.0,
				x_per_div,
				10**(-exp),
			)
		#update x units
		self.plotter.set_x_label('Frequency', prefix+'Hz')
		#update y grid
		duration = float(num_lines)/frame_rate
		y_per_div = common.get_clean_num(duration/y_divs)
		self.plotter.set_y_grid(0, duration, y_per_div)
		#update y units
		self.plotter.set_y_label('Time', 's')
		#update plotter
		self.plotter.update()
