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
import forms

##################################################
# Constants
##################################################
SLIDER_STEPS = 100
AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP = -3, 0
DEFAULT_FRAME_RATE = gr.prefs().get_long('wxgui', 'waterfall_rate', 30)
DEFAULT_COLOR_MODE = gr.prefs().get_string('wxgui', 'waterfall_color', 'rgb1')
DEFAULT_WIN_SIZE = (600, 300)
DIV_LEVELS = (1, 2, 5, 10, 20)
MIN_DYNAMIC_RANGE, MAX_DYNAMIC_RANGE = 10, 200
DYNAMIC_RANGE_STEP = 10.
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
		parent[SHOW_CONTROL_PANEL_KEY] = True
		parent.subscribe(SHOW_CONTROL_PANEL_KEY, self.Show)
		control_box = wx.BoxSizer(wx.VERTICAL)
		control_box.AddStretchSpacer()
		options_box = forms.static_box_sizer(
			parent=self, sizer=control_box, label='Options',
			bold=True, orient=wx.VERTICAL,
		)
		#average
		forms.check_box(
			sizer=options_box, parent=self, label='Average',
			ps=parent, key=AVERAGE_KEY,
		)
		avg_alpha_text = forms.static_text(
			sizer=options_box, parent=self, label='Avg Alpha',
			converter=forms.float_converter(lambda x: '%.4f'%x),
			ps=parent, key=AVG_ALPHA_KEY, width=50,
		)
		avg_alpha_slider = forms.log_slider(
			sizer=options_box, parent=self,
			min_exp=AVG_ALPHA_MIN_EXP,
			max_exp=AVG_ALPHA_MAX_EXP,
			num_steps=SLIDER_STEPS,
			ps=parent, key=AVG_ALPHA_KEY,
		)
		for widget in (avg_alpha_text, avg_alpha_slider):
			parent.subscribe(AVERAGE_KEY, widget.Enable)
			widget.Enable(parent[AVERAGE_KEY])
		#begin axes box
		control_box.AddStretchSpacer()
		axes_box = forms.static_box_sizer(
			parent=self, sizer=control_box, label='Axes Options',
			bold=True, orient=wx.VERTICAL,
		)
		#num lines buttons
		forms.incr_decr_buttons(
			parent=self, sizer=axes_box, label='Time Scale',
			on_incr=self._on_incr_time_scale, on_decr=self._on_decr_time_scale,
		)
		#dyanmic range buttons
		forms.incr_decr_buttons(
			parent=self, sizer=axes_box, label='Dyn Range',
			on_incr=self._on_incr_dynamic_range, on_decr=self._on_decr_dynamic_range,
		)
		#ref lvl buttons
		forms.incr_decr_buttons(
			parent=self, sizer=axes_box, label='Ref Level',
			on_incr=self._on_incr_ref_level, on_decr=self._on_decr_ref_level,
		)
		#color mode
		forms.drop_down(
			parent=self, sizer=axes_box, width=100,
			ps=parent, key=COLOR_MODE_KEY, label='Color',
			choices=map(lambda x: x[1], COLOR_MODES),
			labels=map(lambda x: x[0], COLOR_MODES),
		)
		#autoscale
		forms.single_button(
			parent=self, sizer=axes_box, label='Autoscale',
			callback=self.parent.autoscale,
		)
		#clear
		control_box.AddStretchSpacer()
		forms.single_button(
			parent=self, sizer=control_box, label='Clear',
			callback=self._on_clear_button,
		)
		#run/stop
		forms.toggle_button(
			sizer=control_box, parent=self,
			true_label='Stop', false_label='Run',
			ps=parent, key=RUNNING_KEY,
		)
		#set sizer
		self.SetSizerAndFit(control_box)

	##################################################
	# Event handlers
	##################################################
	def _on_clear_button(self, event):
		self.parent[NUM_LINES_KEY] = self.parent[NUM_LINES_KEY]
	def _on_incr_dynamic_range(self, event):
		self.parent[DYNAMIC_RANGE_KEY] = min(MAX_DYNAMIC_RANGE, common.get_clean_incr(self.parent[DYNAMIC_RANGE_KEY]))
	def _on_decr_dynamic_range(self, event):
		self.parent[DYNAMIC_RANGE_KEY] = max(MIN_DYNAMIC_RANGE, common.get_clean_decr(self.parent[DYNAMIC_RANGE_KEY]))
	def _on_incr_ref_level(self, event):
		self.parent[REF_LEVEL_KEY] = self.parent[REF_LEVEL_KEY] + self.parent[DYNAMIC_RANGE_KEY]/DYNAMIC_RANGE_STEP
	def _on_decr_ref_level(self, event):
		self.parent[REF_LEVEL_KEY] = self.parent[REF_LEVEL_KEY] - self.parent[DYNAMIC_RANGE_KEY]/DYNAMIC_RANGE_STEP
	def _on_incr_time_scale(self, event):
		old_rate = self.parent[FRAME_RATE_KEY]
		self.parent[FRAME_RATE_KEY] *= 0.75
		if self.parent[FRAME_RATE_KEY] < 1.0:
			self.parent[FRAME_RATE_KEY] = 1.0
		
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
		#plotter listeners
		self.subscribe(COLOR_MODE_KEY, self.plotter.set_color_mode)
		self.subscribe(NUM_LINES_KEY, self.plotter.set_num_lines)
		#initialize values
		self[DYNAMIC_RANGE_KEY] = dynamic_range
		self[NUM_LINES_KEY] = num_lines
		self[Y_DIVS_KEY] = 8
		self[X_DIVS_KEY] = 8 #approximate
		self[REF_LEVEL_KEY] = ref_level
		self[BASEBAND_FREQ_KEY] = baseband_freq
		self[COLOR_MODE_KEY] = COLOR_MODES[0][1]
		self[COLOR_MODE_KEY] = DEFAULT_COLOR_MODE
		self[RUNNING_KEY] = True
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
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
		min_level, max_level = common.get_min_max_fft(self.samples)
		#set the range and level
		self[DYNAMIC_RANGE_KEY] = common.get_clean_num(max_level - min_level)
		self[REF_LEVEL_KEY] = DYNAMIC_RANGE_STEP*round(.5+max_level/DYNAMIC_RANGE_STEP)

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
		if self.real: samples = samples[:(num_samps+1)/2]
		else: samples = numpy.concatenate((samples[num_samps/2+1:], samples[:(num_samps+1)/2]))
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
		if frame_rate < 1.0 :
			frame_rate = 1.0
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
