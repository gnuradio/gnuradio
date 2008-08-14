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
import time
import pubsub
from constants import *

##################################################
# Constants
##################################################
DEFAULT_FRAME_RATE = 30
DEFAULT_WIN_SIZE = (600, 300)
DEFAULT_V_SCALE = 1000
TRIGGER_MODES = (
	('Off', 0),
	('Neg', -1),
	('Pos', +1),
)
TRIGGER_LEVELS = (
	('Auto', None),
	('+High', 0.75),
	('+Med', 0.5),
	('+Low', 0.25),
	('Zero', 0.0),
	('-Low', -0.25),
	('-Med', -0.5),
	('-High', -0.75),
)
CHANNEL_COLOR_SPECS = (
	(0, 0, 1),
	(0, 1, 0),
	(1, 0, 0),
	(1, 0, 1),
)
AUTORANGE_UPDATE_RATE = 0.5 #sec

##################################################
# Scope window control panel
##################################################
class control_panel(wx.Panel):
	"""!
	A control panel with wx widgits to control the plotter and scope block.
	"""
	def __init__(self, parent):
		"""!
		Create a new control panel.
		@param parent the wx parent window
		"""
		self.parent = parent
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		self.control_box = control_box = wx.BoxSizer(wx.VERTICAL)
		#trigger options
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Trigger Options'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		#trigger mode
		self.trigger_mode_chooser = common.DropDownController(self, 'Mode', TRIGGER_MODES, parent, TRIGGER_MODE_KEY)
		control_box.Add(self.trigger_mode_chooser, 0, wx.EXPAND)
		#trigger level
		self.trigger_level_chooser = common.DropDownController(self, 'Level', TRIGGER_LEVELS, parent, TRIGGER_LEVEL_KEY)
		parent.subscribe(TRIGGER_MODE_KEY, lambda x: self.trigger_level_chooser.Disable(x==0))
		control_box.Add(self.trigger_level_chooser, 0, wx.EXPAND)
		#trigger channel
		choices = [('Ch%d'%(i+1), i) for i in range(parent.num_inputs)]
		self.trigger_channel_chooser = common.DropDownController(self, 'Channel', choices, parent, TRIGGER_CHANNEL_KEY)
		parent.subscribe(TRIGGER_MODE_KEY, lambda x: self.trigger_channel_chooser.Disable(x==0))
		control_box.Add(self.trigger_channel_chooser, 0, wx.EXPAND)
		#axes options
		SPACING = 15
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Axes Options'), 0, wx.ALIGN_CENTER)
		control_box.AddSpacer(2)
		##################################################
		# Scope Mode Box
		##################################################
		self.scope_mode_box = wx.BoxSizer(wx.VERTICAL)
		control_box.Add(self.scope_mode_box, 0, wx.EXPAND)
		#x axis divs
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		self.scope_mode_box.Add(hbox, 0, wx.EXPAND)
		hbox.Add(wx.StaticText(self, -1, ' Secs/Div '), 1, wx.ALIGN_CENTER_VERTICAL)
		x_buttons = common.IncrDecrButtons(self, self._on_incr_t_divs, self._on_decr_t_divs)
		hbox.Add(x_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		hbox.AddSpacer(SPACING)
		#y axis divs
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		self.scope_mode_box.Add(hbox, 0, wx.EXPAND)
		hbox.Add(wx.StaticText(self, -1, ' Units/Div '), 1, wx.ALIGN_CENTER_VERTICAL)
		y_buttons = common.IncrDecrButtons(self, self._on_incr_y_divs, self._on_decr_y_divs)
		parent.subscribe(AUTORANGE_KEY, y_buttons.Disable)
		hbox.Add(y_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		hbox.AddSpacer(SPACING)
		#y axis ref lvl
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		self.scope_mode_box.Add(hbox, 0, wx.EXPAND)
		hbox.Add(wx.StaticText(self, -1, ' Y Offset '), 1, wx.ALIGN_CENTER_VERTICAL)
		y_off_buttons = common.IncrDecrButtons(self, self._on_incr_y_off, self._on_decr_y_off)
		parent.subscribe(AUTORANGE_KEY, y_off_buttons.Disable)
		hbox.Add(y_off_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		hbox.AddSpacer(SPACING)
		##################################################
		# XY Mode Box
		##################################################
		self.xy_mode_box = wx.BoxSizer(wx.VERTICAL)
		control_box.Add(self.xy_mode_box, 0, wx.EXPAND)
		#x and y channel
		CHOOSER_WIDTH = 60
		CENTER_SPACING = 10
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		self.xy_mode_box.Add(hbox, 0, wx.EXPAND)
		choices = [('Ch%d'%(i+1), i) for i in range(parent.num_inputs)]
		self.channel_x_chooser = common.DropDownController(self, 'X Ch', choices, parent, SCOPE_X_CHANNEL_KEY, (CHOOSER_WIDTH, -1))
		hbox.Add(self.channel_x_chooser, 0, wx.EXPAND)
		hbox.AddSpacer(CENTER_SPACING)
		self.channel_y_chooser = common.DropDownController(self, 'Y Ch', choices, parent, SCOPE_Y_CHANNEL_KEY, (CHOOSER_WIDTH, -1))
		hbox.Add(self.channel_y_chooser, 0, wx.EXPAND)
		#div controls
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		self.xy_mode_box.Add(hbox, 0, wx.EXPAND)
		hbox.Add(wx.StaticText(self, -1, ' X/Div '), 1, wx.ALIGN_CENTER_VERTICAL)
		x_buttons = common.IncrDecrButtons(self, self._on_incr_x_divs, self._on_decr_x_divs)
		parent.subscribe(AUTORANGE_KEY, x_buttons.Disable)
		hbox.Add(x_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		hbox.AddSpacer(CENTER_SPACING)
		hbox.Add(wx.StaticText(self, -1, ' Y/Div '), 1, wx.ALIGN_CENTER_VERTICAL)
		y_buttons = common.IncrDecrButtons(self, self._on_incr_y_divs, self._on_decr_y_divs)
		parent.subscribe(AUTORANGE_KEY, y_buttons.Disable)
		hbox.Add(y_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		#offset controls
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		self.xy_mode_box.Add(hbox, 0, wx.EXPAND)
		hbox.Add(wx.StaticText(self, -1, ' X Off '), 1, wx.ALIGN_CENTER_VERTICAL)
		x_off_buttons = common.IncrDecrButtons(self, self._on_incr_x_off, self._on_decr_x_off)
		parent.subscribe(AUTORANGE_KEY, x_off_buttons.Disable)
		hbox.Add(x_off_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		hbox.AddSpacer(CENTER_SPACING)
		hbox.Add(wx.StaticText(self, -1, ' Y Off '), 1, wx.ALIGN_CENTER_VERTICAL)
		y_off_buttons = common.IncrDecrButtons(self, self._on_incr_y_off, self._on_decr_y_off)
		parent.subscribe(AUTORANGE_KEY, y_off_buttons.Disable)
		hbox.Add(y_off_buttons, 0, wx.ALIGN_CENTER_VERTICAL)
		##################################################
		# End Special Boxes
		##################################################
		#misc options
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Range Options'), 0, wx.ALIGN_CENTER)
		#ac couple check box
		self.ac_couple_check_box = common.CheckBoxController(self, 'AC Couple', parent, AC_COUPLE_KEY)
		control_box.Add(self.ac_couple_check_box, 0, wx.ALIGN_LEFT)
		#autorange check box
		self.autorange_check_box = common.CheckBoxController(self, 'Autorange', parent, AUTORANGE_KEY)
		control_box.Add(self.autorange_check_box, 0, wx.ALIGN_LEFT)
		#run/stop
		control_box.AddStretchSpacer()
		self.scope_xy_mode_button = common.ToggleButtonController(self, parent, SCOPE_XY_MODE_KEY, 'Scope Mode', 'X:Y Mode')
		parent.subscribe(SCOPE_XY_MODE_KEY, self._on_scope_xy_mode)
		control_box.Add(self.scope_xy_mode_button, 0, wx.EXPAND)
		#run/stop
		self.run_button = common.ToggleButtonController(self, parent, RUNNING_KEY, 'Stop', 'Run')
		control_box.Add(self.run_button, 0, wx.EXPAND)
		#set sizer
		self.SetSizerAndFit(control_box)

	##################################################
	# Event handlers
	##################################################
	def _on_scope_xy_mode(self, mode):
		self.scope_mode_box.ShowItems(not mode)
		self.xy_mode_box.ShowItems(mode)
		self.control_box.Layout()
	#incr/decr divs
	def _on_incr_t_divs(self, event):
		self.parent.set_t_per_div(
			common.get_clean_incr(self.parent[T_PER_DIV_KEY]))
	def _on_decr_t_divs(self, event):
		self.parent.set_t_per_div(
			common.get_clean_decr(self.parent[T_PER_DIV_KEY]))
	def _on_incr_x_divs(self, event):
		self.parent.set_x_per_div(
			common.get_clean_incr(self.parent[X_PER_DIV_KEY]))
	def _on_decr_x_divs(self, event):
		self.parent.set_x_per_div(
			common.get_clean_decr(self.parent[X_PER_DIV_KEY]))
	def _on_incr_y_divs(self, event):
		self.parent.set_y_per_div(
			common.get_clean_incr(self.parent[Y_PER_DIV_KEY]))
	def _on_decr_y_divs(self, event):
		self.parent.set_y_per_div(
			common.get_clean_decr(self.parent[Y_PER_DIV_KEY]))
	#incr/decr offset
	def _on_incr_t_off(self, event):
		self.parent.set_t_off(
			self.parent[T_OFF_KEY] + self.parent[T_PER_DIV_KEY])
	def _on_decr_t_off(self, event):
		self.parent.set_t_off(
			self.parent[T_OFF_KEY] - self.parent[T_PER_DIV_KEY])
	def _on_incr_x_off(self, event):
		self.parent.set_x_off(
			self.parent[X_OFF_KEY] + self.parent[X_PER_DIV_KEY])
	def _on_decr_x_off(self, event):
		self.parent.set_x_off(
			self.parent[X_OFF_KEY] - self.parent[X_PER_DIV_KEY])
	def _on_incr_y_off(self, event):
		self.parent.set_y_off(
			self.parent[Y_OFF_KEY] + self.parent[Y_PER_DIV_KEY])
	def _on_decr_y_off(self, event):
		self.parent.set_y_off(
			self.parent[Y_OFF_KEY] - self.parent[Y_PER_DIV_KEY])

##################################################
# Scope window with plotter and control panel
##################################################
class scope_window(wx.Panel, pubsub.pubsub, common.prop_setter):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		frame_rate,
		num_inputs,
		sample_rate_key,
		t_scale,
		v_scale,
		ac_couple,
		xy_mode,
		scope_trigger_level_key,
		scope_trigger_mode_key,
		scope_trigger_channel_key,
		msg_key,
	):
		pubsub.pubsub.__init__(self)
		#check num inputs
		assert num_inputs <= len(CHANNEL_COLOR_SPECS)
		#setup
		self.ext_controller = controller
		self.num_inputs = num_inputs
		self.sample_rate_key = sample_rate_key
		autorange = v_scale is None
		self.autorange_ts = 0
		if v_scale is None: v_scale = 1
		self.frame_rate_ts = 0
		self._init = False #HACK
		#scope keys
		self.scope_trigger_level_key = scope_trigger_level_key
		self.scope_trigger_mode_key = scope_trigger_mode_key
		self.scope_trigger_channel_key = scope_trigger_channel_key
		#init panel and plot
		wx.Panel.__init__(self, parent, -1, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.channel_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.set_title(title)
		self.plotter.enable_legend(True)
		self.plotter.enable_point_label(True)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#initial setup
		self._register_set_prop(self, RUNNING_KEY, True)
		self._register_set_prop(self, AC_COUPLE_KEY, ac_couple)
		self._register_set_prop(self, SCOPE_XY_MODE_KEY, xy_mode)
		self._register_set_prop(self, AUTORANGE_KEY, autorange)
		self._register_set_prop(self, T_PER_DIV_KEY, t_scale)
		self._register_set_prop(self, X_PER_DIV_KEY, v_scale)
		self._register_set_prop(self, Y_PER_DIV_KEY, v_scale)
		self._register_set_prop(self, T_OFF_KEY, 0)
		self._register_set_prop(self, X_OFF_KEY, 0)
		self._register_set_prop(self, Y_OFF_KEY, 0)
		self._register_set_prop(self, T_DIVS_KEY, 8)
		self._register_set_prop(self, X_DIVS_KEY, 8)
		self._register_set_prop(self, Y_DIVS_KEY, 8)
		self._register_set_prop(self, SCOPE_X_CHANNEL_KEY, 0)
		self._register_set_prop(self, SCOPE_Y_CHANNEL_KEY, num_inputs-1)
		self._register_set_prop(self, FRAME_RATE_KEY, frame_rate)
		self._register_set_prop(self, TRIGGER_CHANNEL_KEY, 0)
		self._register_set_prop(self, TRIGGER_MODE_KEY, 1)
		self._register_set_prop(self, TRIGGER_LEVEL_KEY, None)
		#register events
		self.ext_controller.subscribe(msg_key, self.handle_msg)
		for key in (
			T_PER_DIV_KEY, X_PER_DIV_KEY, Y_PER_DIV_KEY,
			T_OFF_KEY, X_OFF_KEY, Y_OFF_KEY,
			T_DIVS_KEY, X_DIVS_KEY, Y_DIVS_KEY,
			SCOPE_XY_MODE_KEY,
			SCOPE_X_CHANNEL_KEY,
			SCOPE_Y_CHANNEL_KEY,
			AUTORANGE_KEY,
			AC_COUPLE_KEY,
		): self.subscribe(key, self.update_grid)
		#initial update, dont do this here, wait for handle_msg #HACK
		#self.update_grid()

	def handle_msg(self, msg):
		"""!
		Handle the message from the scope sink message queue.
		Plot the list of arrays of samples onto the grid.
		Each samples array gets its own channel.
		@param msg the time domain data as a character array
		"""
		if not self[RUNNING_KEY]: return
		#check time elapsed
		if time.time() - self.frame_rate_ts < 1.0/self[FRAME_RATE_KEY]: return
		#convert to floating point numbers
		samples = numpy.fromstring(msg, numpy.float32)
		samps_per_ch = len(samples)/self.num_inputs
		self.sampleses = [samples[samps_per_ch*i:samps_per_ch*(i+1)] for i in range(self.num_inputs)]
		if not self._init: #HACK
			self._init = True
			self.update_grid()
		#handle samples
		self.handle_samples()
		self.frame_rate_ts = time.time()

	def handle_samples(self):
		"""!
		Handle the cached samples from the scope input.
		Perform ac coupling, triggering, and auto ranging.
		"""
		sampleses = self.sampleses
		#trigger level (must do before ac coupling)
		self.ext_controller[self.scope_trigger_channel_key] = self[TRIGGER_CHANNEL_KEY]
		self.ext_controller[self.scope_trigger_mode_key] = self[TRIGGER_MODE_KEY]
		trigger_level = self[TRIGGER_LEVEL_KEY]
		if trigger_level is None: self.ext_controller[self.scope_trigger_level_key] = ''
		else:
			samples = sampleses[self[TRIGGER_CHANNEL_KEY]]
			self.ext_controller[self.scope_trigger_level_key] = \
			trigger_level*(numpy.max(samples)-numpy.min(samples))/2 + numpy.average(samples)
		#ac coupling
		if self[AC_COUPLE_KEY]:
			sampleses = [samples - numpy.average(samples) for samples in sampleses]
		if self[SCOPE_XY_MODE_KEY]:
			x_samples = sampleses[self[SCOPE_X_CHANNEL_KEY]]
			y_samples = sampleses[self[SCOPE_Y_CHANNEL_KEY]]
			#autorange
			if self[AUTORANGE_KEY] and time.time() - self.autorange_ts > AUTORANGE_UPDATE_RATE:
				x_min, x_max = common.get_min_max(x_samples)
				y_min, y_max = common.get_min_max(y_samples)
				#adjust the x per div
				x_per_div = common.get_clean_num((x_max-x_min)/self[X_DIVS_KEY])
				if x_per_div != self[X_PER_DIV_KEY]: self.set_x_per_div(x_per_div)
				#adjust the x offset
				x_off = x_per_div*round((x_max+x_min)/2/x_per_div)
				if x_off != self[X_OFF_KEY]: self.set_x_off(x_off)
				#adjust the y per div
				y_per_div = common.get_clean_num((y_max-y_min)/self[Y_DIVS_KEY])
				if y_per_div != self[Y_PER_DIV_KEY]: self.set_y_per_div(y_per_div)
				#adjust the y offset
				y_off = y_per_div*round((y_max+y_min)/2/y_per_div)
				if y_off != self[Y_OFF_KEY]: self.set_y_off(y_off)
				self.autorange_ts = time.time()
			#plot xy channel
			self.plotter.set_waveform(
				channel='XY',
				samples=(x_samples, y_samples),
				color_spec=CHANNEL_COLOR_SPECS[0],
			)
			#turn off each waveform
			for i, samples in enumerate(sampleses):
				self.plotter.set_waveform(
					channel='Ch%d'%(i+1),
					samples=[],
					color_spec=CHANNEL_COLOR_SPECS[i],
				)
		else:
			#autorange
			if self[AUTORANGE_KEY] and time.time() - self.autorange_ts > AUTORANGE_UPDATE_RATE:
				bounds = [common.get_min_max(samples) for samples in sampleses]
				y_min = numpy.min(*[bound[0] for bound in bounds])
				y_max = numpy.max(*[bound[1] for bound in bounds])
				#adjust the y per div
				y_per_div = common.get_clean_num((y_max-y_min)/self[Y_DIVS_KEY])
				if y_per_div != self[Y_PER_DIV_KEY]: self.set_y_per_div(y_per_div)
				#adjust the y offset
				y_off = y_per_div*round((y_max+y_min)/2/y_per_div)
				if y_off != self[Y_OFF_KEY]: self.set_y_off(y_off)
				self.autorange_ts = time.time()
			#plot each waveform
			for i, samples in enumerate(sampleses):
				#number of samples to scale to the screen
				num_samps = int(self[T_PER_DIV_KEY]*self[T_DIVS_KEY]*self.ext_controller[self.sample_rate_key])
				#handle num samps out of bounds
				if num_samps > len(samples):
					self.set_t_per_div(
						common.get_clean_decr(self[T_PER_DIV_KEY]))
				elif num_samps < 2:
					self.set_t_per_div(
						common.get_clean_incr(self[T_PER_DIV_KEY]))
					num_samps = 0
				else:
					#plot samples
					self.plotter.set_waveform(
						channel='Ch%d'%(i+1),
						samples=samples[:num_samps],
						color_spec=CHANNEL_COLOR_SPECS[i],
					)
			#turn XY channel off
			self.plotter.set_waveform(
				channel='XY',
				samples=[],
				color_spec=CHANNEL_COLOR_SPECS[0],
			)
		#update the plotter
		self.plotter.update()

	def update_grid(self, *args):
		"""!
		Update the grid to reflect the current settings:
		xy divisions, xy offset, xy mode setting
		"""
		#grid parameters
		t_per_div = self[T_PER_DIV_KEY]
		x_per_div = self[X_PER_DIV_KEY]
		y_per_div = self[Y_PER_DIV_KEY]
		t_off = self[T_OFF_KEY]
		x_off = self[X_OFF_KEY]
		y_off = self[Y_OFF_KEY]
		t_divs = self[T_DIVS_KEY]
		x_divs = self[X_DIVS_KEY]
		y_divs = self[Y_DIVS_KEY]
		if self[SCOPE_XY_MODE_KEY]:
			#update the x axis
			self.plotter.set_x_label('Ch%d'%(self[SCOPE_X_CHANNEL_KEY]+1))
			self.plotter.set_x_grid(
				-1*x_per_div*x_divs/2.0 + x_off,
				x_per_div*x_divs/2.0 + x_off,
				x_per_div,
			)
			#update the y axis
			self.plotter.set_y_label('Ch%d'%(self[SCOPE_Y_CHANNEL_KEY]+1))
			self.plotter.set_y_grid(
				-1*y_per_div*y_divs/2.0 + y_off,
				y_per_div*y_divs/2.0 + y_off,
				y_per_div,
			)
		else:
			#update the t axis
			coeff, exp, prefix = common.get_si_components(t_per_div*t_divs + t_off)
			self.plotter.set_x_label('Time', prefix+'s')
			self.plotter.set_x_grid(
				t_off,
				t_per_div*t_divs + t_off,
				t_per_div,
				10**(-exp),
			)
			#update the y axis
			self.plotter.set_y_label('Counts')
			self.plotter.set_y_grid(
				-1*y_per_div*y_divs/2.0 + y_off,
				y_per_div*y_divs/2.0 + y_off,
				y_per_div,
			)
		#redraw current sample
		self.handle_samples()
