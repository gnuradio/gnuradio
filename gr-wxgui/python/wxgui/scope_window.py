#
# Copyright 2008,2010 Free Software Foundation, Inc.
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
from gnuradio import gr #for gr.prefs, trigger modes
from gnuradio import wxgui
import forms

##################################################
# Constants
##################################################
DEFAULT_FRAME_RATE = gr.prefs().get_long('wxgui', 'scope_rate', 30)
PERSIST_ALPHA_MIN_EXP, PERSIST_ALPHA_MAX_EXP = -2, 0
SLIDER_STEPS = 100
DEFAULT_TRIG_MODE = gr.prefs().get_long('wxgui', 'trig_mode', wxgui.TRIG_MODE_AUTO)
DEFAULT_WIN_SIZE = (600, 300)
COUPLING_MODES = (
	('DC', False),
	('AC', True),
)
TRIGGER_MODES = (
	('Freerun', wxgui.TRIG_MODE_FREE),
	('Auto', wxgui.TRIG_MODE_AUTO),
	('Normal', wxgui.TRIG_MODE_NORM),
	('Stripchart', wxgui.TRIG_MODE_STRIPCHART),
)
TRIGGER_SLOPES = (
	('Pos +', wxgui.TRIG_SLOPE_POS),
	('Neg -', wxgui.TRIG_SLOPE_NEG),
)
CHANNEL_COLOR_SPECS = (
	(0.3, 0.3, 1.0),
	(0.0, 0.8, 0.0),
	(1.0, 0.0, 0.0),
	(0.8, 0.0, 0.8),
        (0.7, 0.7, 0.0),
        (0.15, 0.90, 0.98),

)
TRIGGER_COLOR_SPEC = (1.0, 0.4, 0.0)
AUTORANGE_UPDATE_RATE = 0.5 #sec
MARKER_TYPES = (
	('Line Link', None),
	('Dot Large', 3.0),
	('Dot Med', 2.0),
	('Dot Small', 1.0),
	('None', 0.0),
)
DEFAULT_MARKER_TYPE = None

##################################################
# Scope window control panel
##################################################
class control_panel(wx.Panel):
	"""
	A control panel with wx widgits to control the plotter and scope block.
	"""
	def __init__(self, parent):
		"""
		Create a new control panel.

                Args:
		    parent: the wx parent window
		"""
		WIDTH = 90
		self.parent = parent
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		parent[SHOW_CONTROL_PANEL_KEY] = True
		parent.subscribe(SHOW_CONTROL_PANEL_KEY, self.Show)
		control_box = wx.BoxSizer(wx.VERTICAL)

		##################################################
		# Persistence
		##################################################

		forms.check_box(
			sizer=control_box, parent=self, label='Persistence',
			ps=parent, key=USE_PERSISTENCE_KEY,
		)
		#static text and slider for analog alpha
		persist_alpha_text = forms.static_text(
			sizer=control_box, parent=self, label='Analog Alpha',
			converter=forms.float_converter(lambda x: '%.4f'%x),
			ps=parent, key=PERSIST_ALPHA_KEY, width=50,
		)
		persist_alpha_slider = forms.log_slider(
			sizer=control_box, parent=self,
			min_exp=PERSIST_ALPHA_MIN_EXP,
			max_exp=PERSIST_ALPHA_MAX_EXP,
			num_steps=SLIDER_STEPS,
			ps=parent, key=PERSIST_ALPHA_KEY,
		)
		for widget in (persist_alpha_text, persist_alpha_slider):
			parent.subscribe(USE_PERSISTENCE_KEY, widget.Enable)
			widget.Enable(parent[USE_PERSISTENCE_KEY])
			parent.subscribe(USE_PERSISTENCE_KEY, widget.ShowItems)
                        #always show initially, so room is reserved for them
			widget.ShowItems(True) # (parent[USE_PERSISTENCE_KEY])

                parent.subscribe(USE_PERSISTENCE_KEY, self._update_layout)

		##################################################
		# Axes Options
		##################################################
		control_box.AddStretchSpacer()
		axes_options_box = forms.static_box_sizer(
			parent=self, sizer=control_box, label='Axes Options',
			bold=True, orient=wx.VERTICAL,
		)
		##################################################
		# Scope Mode Box
		##################################################
		scope_mode_box = wx.BoxSizer(wx.VERTICAL)
		axes_options_box.Add(scope_mode_box, 0, wx.EXPAND)
		#x axis divs
		forms.incr_decr_buttons(
			parent=self, sizer=scope_mode_box, label='Secs/Div',
			on_incr=self._on_incr_t_divs, on_decr=self._on_decr_t_divs,
		)
		#y axis divs
		y_buttons_scope = forms.incr_decr_buttons(
			parent=self, sizer=scope_mode_box, label='Counts/Div',
			on_incr=self._on_incr_y_divs, on_decr=self._on_decr_y_divs,
		)
		#y axis ref lvl
		y_off_buttons_scope = forms.incr_decr_buttons(
			parent=self, sizer=scope_mode_box, label='Y Offset',
			on_incr=self._on_incr_y_off, on_decr=self._on_decr_y_off,
		)
		#t axis ref lvl
		scope_mode_box.AddSpacer(5)
		forms.slider(
			parent=self, sizer=scope_mode_box,
			ps=parent, key=T_FRAC_OFF_KEY, label='T Offset',
			minimum=0, maximum=1, num_steps=1000,
		)
		scope_mode_box.AddSpacer(5)
		##################################################
		# XY Mode Box
		##################################################
		xy_mode_box = wx.BoxSizer(wx.VERTICAL)
		axes_options_box.Add(xy_mode_box, 0, wx.EXPAND)
		#x div controls
		x_buttons = forms.incr_decr_buttons(
			parent=self, sizer=xy_mode_box, label='X/Div',
			on_incr=self._on_incr_x_divs, on_decr=self._on_decr_x_divs,
		)
		#y div controls
		y_buttons = forms.incr_decr_buttons(
			parent=self, sizer=xy_mode_box, label='Y/Div',
			on_incr=self._on_incr_y_divs, on_decr=self._on_decr_y_divs,
		)
		#x offset controls
		x_off_buttons = forms.incr_decr_buttons(
			parent=self, sizer=xy_mode_box, label='X Off',
			on_incr=self._on_incr_x_off, on_decr=self._on_decr_x_off,
		)
		#y offset controls
		y_off_buttons = forms.incr_decr_buttons(
			parent=self, sizer=xy_mode_box, label='Y Off',
			on_incr=self._on_incr_y_off, on_decr=self._on_decr_y_off,
		)
		for widget in (y_buttons_scope, y_off_buttons_scope, x_buttons, y_buttons, x_off_buttons, y_off_buttons):
			parent.subscribe(AUTORANGE_KEY, widget.Disable)
			widget.Disable(parent[AUTORANGE_KEY])
		xy_mode_box.ShowItems(False)
		#autorange check box
		forms.check_box(
			parent=self, sizer=axes_options_box, label='Autorange',
			ps=parent, key=AUTORANGE_KEY,
		)
		##################################################
		# Channel Options
		##################################################
		TRIGGER_PAGE_INDEX = parent.num_inputs
		XY_PAGE_INDEX = parent.num_inputs+1
		control_box.AddStretchSpacer()
		chan_options_box = forms.static_box_sizer(
			parent=self, sizer=control_box, label='Channel Options',
			bold=True, orient=wx.VERTICAL,
		)
		options_notebook = wx.Notebook(self)
		options_notebook_args = list()
		CHANNELS = [('Ch %d'%(i+1), i) for i in range(parent.num_inputs)]
		##################################################
		# Channel Menu Boxes
		##################################################
		for i in range(parent.num_inputs):
			channel_menu_panel = wx.Panel(options_notebook)
			options_notebook_args.append((channel_menu_panel, i, 'Ch%d'%(i+1)))
			channel_menu_box = wx.BoxSizer(wx.VERTICAL)
			channel_menu_panel.SetSizer(channel_menu_box)
			#ac couple check box
			channel_menu_box.AddStretchSpacer()
			forms.drop_down(
				parent=channel_menu_panel, sizer=channel_menu_box,
				ps=parent, key=common.index_key(AC_COUPLE_KEY, i),
				choices=map(lambda x: x[1], COUPLING_MODES),
				labels=map(lambda x: x[0], COUPLING_MODES),
				label='Coupling', width=WIDTH,
			)
			#marker
			channel_menu_box.AddStretchSpacer()
			forms.drop_down(
				parent=channel_menu_panel, sizer=channel_menu_box,
				ps=parent, key=common.index_key(MARKER_KEY, i),
				choices=map(lambda x: x[1], MARKER_TYPES),
				labels=map(lambda x: x[0], MARKER_TYPES),
				label='Marker', width=WIDTH,
			)
			channel_menu_box.AddStretchSpacer()
		##################################################
		# Trigger Menu Box
		##################################################
		trigger_menu_panel = wx.Panel(options_notebook)
		options_notebook_args.append((trigger_menu_panel, TRIGGER_PAGE_INDEX, 'Trig'))
		trigger_menu_box = wx.BoxSizer(wx.VERTICAL)
		trigger_menu_panel.SetSizer(trigger_menu_box)
		#trigger mode
		forms.drop_down(
			parent=trigger_menu_panel, sizer=trigger_menu_box,
			ps=parent, key=TRIGGER_MODE_KEY,
			choices=map(lambda x: x[1], TRIGGER_MODES),
			labels=map(lambda x: x[0], TRIGGER_MODES),
			label='Mode', width=WIDTH,
		)
		#trigger slope
		trigger_slope_chooser = forms.drop_down(
			parent=trigger_menu_panel, sizer=trigger_menu_box,
			ps=parent, key=TRIGGER_SLOPE_KEY,
			choices=map(lambda x: x[1], TRIGGER_SLOPES),
			labels=map(lambda x: x[0], TRIGGER_SLOPES),
			label='Slope', width=WIDTH,
		)
		#trigger channel
		trigger_channel_chooser = forms.drop_down(
			parent=trigger_menu_panel, sizer=trigger_menu_box,
			ps=parent, key=TRIGGER_CHANNEL_KEY,
			choices=map(lambda x: x[1], CHANNELS),
			labels=map(lambda x: x[0], CHANNELS),
			label='Channel', width=WIDTH,
		)
		#trigger level
		hbox = wx.BoxSizer(wx.HORIZONTAL)
		trigger_menu_box.Add(hbox, 0, wx.EXPAND)
		hbox.Add(wx.StaticText(trigger_menu_panel, label='Level:'), 1, wx.ALIGN_CENTER_VERTICAL)
		trigger_level_button = forms.single_button(
			parent=trigger_menu_panel, sizer=hbox, label='50%',
			callback=parent.set_auto_trigger_level, style=wx.BU_EXACTFIT,
		)
		hbox.AddSpacer(WIDTH-60)
		trigger_level_buttons = forms.incr_decr_buttons(
			parent=trigger_menu_panel, sizer=hbox,
			on_incr=self._on_incr_trigger_level, on_decr=self._on_decr_trigger_level,
		)
		def disable_all(trigger_mode):
			for widget in (trigger_slope_chooser, trigger_channel_chooser, trigger_level_buttons, trigger_level_button):
				widget.Disable(trigger_mode == wxgui.TRIG_MODE_FREE)
		parent.subscribe(TRIGGER_MODE_KEY, disable_all)
		disable_all(parent[TRIGGER_MODE_KEY])
		##################################################
		# XY Menu Box
		##################################################
		if parent.num_inputs > 1:
			xy_menu_panel = wx.Panel(options_notebook)
			options_notebook_args.append((xy_menu_panel, XY_PAGE_INDEX, 'XY'))
			xy_menu_box = wx.BoxSizer(wx.VERTICAL)
			xy_menu_panel.SetSizer(xy_menu_box)
			#x and y channel choosers
			xy_menu_box.AddStretchSpacer()
			forms.drop_down(
				parent=xy_menu_panel, sizer=xy_menu_box,
				ps=parent, key=X_CHANNEL_KEY,
				choices=map(lambda x: x[1], CHANNELS),
				labels=map(lambda x: x[0], CHANNELS),
				label='Channel X', width=WIDTH,
			)
			xy_menu_box.AddStretchSpacer()
			forms.drop_down(
				parent=xy_menu_panel, sizer=xy_menu_box,
				ps=parent, key=Y_CHANNEL_KEY,
				choices=map(lambda x: x[1], CHANNELS),
				labels=map(lambda x: x[0], CHANNELS),
				label='Channel Y', width=WIDTH,
			)
			#marker
			xy_menu_box.AddStretchSpacer()
			forms.drop_down(
				parent=xy_menu_panel, sizer=xy_menu_box,
				ps=parent, key=XY_MARKER_KEY,
				choices=map(lambda x: x[1], MARKER_TYPES),
				labels=map(lambda x: x[0], MARKER_TYPES),
				label='Marker', width=WIDTH,
			)
			xy_menu_box.AddStretchSpacer()
		##################################################
		# Setup Options Notebook
		##################################################
		forms.notebook(
			parent=self, sizer=chan_options_box,
			notebook=options_notebook,
			ps=parent, key=CHANNEL_OPTIONS_KEY,
			pages=map(lambda x: x[0], options_notebook_args),
			choices=map(lambda x: x[1], options_notebook_args),
			labels=map(lambda x: x[2], options_notebook_args),
		)
		#gui handling for channel options changing
		def options_notebook_changed(chan_opt):
			try:
				parent[TRIGGER_SHOW_KEY] = chan_opt == TRIGGER_PAGE_INDEX
				parent[XY_MODE_KEY] = chan_opt == XY_PAGE_INDEX
			except wx.PyDeadObjectError: pass
		parent.subscribe(CHANNEL_OPTIONS_KEY, options_notebook_changed)
		#gui handling for xy mode changing
		def xy_mode_changed(mode):
			#ensure xy tab is selected
			if mode and parent[CHANNEL_OPTIONS_KEY] != XY_PAGE_INDEX:
				parent[CHANNEL_OPTIONS_KEY] = XY_PAGE_INDEX
			#ensure xy tab is not selected
			elif not mode and parent[CHANNEL_OPTIONS_KEY] == XY_PAGE_INDEX:
				parent[CHANNEL_OPTIONS_KEY] = 0
			#show/hide control buttons
			scope_mode_box.ShowItems(not mode)
			xy_mode_box.ShowItems(mode)
			control_box.Layout()
		parent.subscribe(XY_MODE_KEY, xy_mode_changed)
		xy_mode_changed(parent[XY_MODE_KEY])
		##################################################
		# Run/Stop Button
		##################################################
		#run/stop
		control_box.AddStretchSpacer()
		forms.toggle_button(
			sizer=control_box, parent=self,
			true_label='Stop', false_label='Run',
			ps=parent, key=RUNNING_KEY,
		)
		#set sizer
		self.SetSizerAndFit(control_box)
		#mouse wheel event
		def on_mouse_wheel(event):
			if not parent[XY_MODE_KEY]:
				if event.GetWheelRotation() < 0: self._on_incr_t_divs(event)
				else: self._on_decr_t_divs(event)
		parent.plotter.Bind(wx.EVT_MOUSEWHEEL, on_mouse_wheel)

	##################################################
	# Event handlers
	##################################################
	#trigger level
	def _on_incr_trigger_level(self, event):
		self.parent[TRIGGER_LEVEL_KEY] += self.parent[Y_PER_DIV_KEY]/3.
	def _on_decr_trigger_level(self, event):
		self.parent[TRIGGER_LEVEL_KEY] -= self.parent[Y_PER_DIV_KEY]/3.
	#incr/decr divs
	def _on_incr_t_divs(self, event):
		self.parent[T_PER_DIV_KEY] = common.get_clean_incr(self.parent[T_PER_DIV_KEY])
	def _on_decr_t_divs(self, event):
		self.parent[T_PER_DIV_KEY] = common.get_clean_decr(self.parent[T_PER_DIV_KEY])
	def _on_incr_x_divs(self, event):
		self.parent[X_PER_DIV_KEY] = common.get_clean_incr(self.parent[X_PER_DIV_KEY])
	def _on_decr_x_divs(self, event):
		self.parent[X_PER_DIV_KEY] = common.get_clean_decr(self.parent[X_PER_DIV_KEY])
	def _on_incr_y_divs(self, event):
		self.parent[Y_PER_DIV_KEY] = common.get_clean_incr(self.parent[Y_PER_DIV_KEY])
	def _on_decr_y_divs(self, event):
		self.parent[Y_PER_DIV_KEY] = common.get_clean_decr(self.parent[Y_PER_DIV_KEY])
	#incr/decr offset
	def _on_incr_x_off(self, event):
		self.parent[X_OFF_KEY] = self.parent[X_OFF_KEY] + self.parent[X_PER_DIV_KEY]
	def _on_decr_x_off(self, event):
		self.parent[X_OFF_KEY] = self.parent[X_OFF_KEY] - self.parent[X_PER_DIV_KEY]
	def _on_incr_y_off(self, event):
		self.parent[Y_OFF_KEY] = self.parent[Y_OFF_KEY] + self.parent[Y_PER_DIV_KEY]
	def _on_decr_y_off(self, event):
		self.parent[Y_OFF_KEY] = self.parent[Y_OFF_KEY] - self.parent[Y_PER_DIV_KEY]

	##################################################
	# subscriber handlers
	##################################################
        def _update_layout(self,key):
          # Just ignore the key value we get
          # we only need to now that the visibility or size of something has changed
          self.parent.Layout()
          #self.parent.Fit()

##################################################
# Scope window with plotter and control panel
##################################################
class scope_window(wx.Panel, pubsub.pubsub):
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
		v_offset,
		xy_mode,
		ac_couple_key,
		trigger_level_key,
		trigger_mode_key,
		trigger_slope_key,
		trigger_channel_key,
		decimation_key,
		msg_key,
                use_persistence,
                persist_alpha,
		trig_mode,
		y_axis_label,
	):
		pubsub.pubsub.__init__(self)
		#check num inputs
		assert num_inputs <= len(CHANNEL_COLOR_SPECS)
		#setup
		self.sampleses = None
		self.num_inputs = num_inputs
		autorange = not v_scale
		self.autorange_ts = 0
		v_scale = v_scale or 1
		self.frame_rate_ts = 0
		#proxy the keys
		self.proxy(MSG_KEY, controller, msg_key)
		self.proxy(SAMPLE_RATE_KEY, controller, sample_rate_key)
		self.proxy(TRIGGER_LEVEL_KEY, controller, trigger_level_key)
		self.proxy(TRIGGER_MODE_KEY, controller, trigger_mode_key)
		self.proxy(TRIGGER_SLOPE_KEY, controller, trigger_slope_key)
		self.proxy(TRIGGER_CHANNEL_KEY, controller, trigger_channel_key)
		self.proxy(DECIMATION_KEY, controller, decimation_key)
		#initialize values
		self[RUNNING_KEY] = True
		self[XY_MARKER_KEY] = 2.0
		self[CHANNEL_OPTIONS_KEY] = 0
		self[XY_MODE_KEY] = xy_mode
		self[X_CHANNEL_KEY] = 0
		self[Y_CHANNEL_KEY] = self.num_inputs-1
		self[AUTORANGE_KEY] = autorange
		self[T_PER_DIV_KEY] = t_scale
		self[X_PER_DIV_KEY] = v_scale
		self[Y_PER_DIV_KEY] = v_scale
		self[T_OFF_KEY] = 0
		self[X_OFF_KEY] = v_offset
		self[Y_OFF_KEY] = v_offset
		self[T_DIVS_KEY] = 8
		self[X_DIVS_KEY] = 8
		self[Y_DIVS_KEY] = 8
		self[Y_AXIS_LABEL] = y_axis_label
		self[FRAME_RATE_KEY] = frame_rate
		self[TRIGGER_LEVEL_KEY] = 0
		self[TRIGGER_CHANNEL_KEY] = 0
		self[TRIGGER_MODE_KEY] = trig_mode

		self[TRIGGER_SLOPE_KEY] = wxgui.TRIG_SLOPE_POS
		self[T_FRAC_OFF_KEY] = 0.5
		self[USE_PERSISTENCE_KEY] = use_persistence
		self[PERSIST_ALPHA_KEY] = persist_alpha

		if self[TRIGGER_MODE_KEY] == wxgui.TRIG_MODE_STRIPCHART:
			self[T_FRAC_OFF_KEY] = 0.0

		for i in range(num_inputs):
			self.proxy(common.index_key(AC_COUPLE_KEY, i), controller, common.index_key(ac_couple_key, i))
		#init panel and plot
		wx.Panel.__init__(self, parent, style=wx.SIMPLE_BORDER)
		self.plotter = plotter.channel_plotter(self)
		self.plotter.SetSize(wx.Size(*size))
		self.plotter.SetSizeHints(*size)
		self.plotter.set_title(title)
		self.plotter.enable_legend(True)
		self.plotter.enable_point_label(True)
		self.plotter.enable_grid_lines(True)
                self.plotter.set_use_persistence(use_persistence)
                self.plotter.set_persist_alpha(persist_alpha)
		#setup the box with plot and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		main_box.Add(self.plotter, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#register events for message
		self.subscribe(MSG_KEY, self.handle_msg)
		#register events for grid
		for key in [common.index_key(MARKER_KEY, i) for i in range(self.num_inputs)] + [
			TRIGGER_LEVEL_KEY, TRIGGER_MODE_KEY,
			T_PER_DIV_KEY, X_PER_DIV_KEY, Y_PER_DIV_KEY,
			T_OFF_KEY, X_OFF_KEY, Y_OFF_KEY,
			T_DIVS_KEY, X_DIVS_KEY, Y_DIVS_KEY,
			XY_MODE_KEY, AUTORANGE_KEY, T_FRAC_OFF_KEY,
			TRIGGER_SHOW_KEY, XY_MARKER_KEY, X_CHANNEL_KEY, Y_CHANNEL_KEY,
		]: self.subscribe(key, self.update_grid)
                #register events for plotter settings
		self.subscribe(USE_PERSISTENCE_KEY, self.plotter.set_use_persistence)
		self.subscribe(PERSIST_ALPHA_KEY, self.plotter.set_persist_alpha)
		#initial update
		self.update_grid()

	def handle_msg(self, msg):
		"""
		Handle the message from the scope sink message queue.
		Plot the list of arrays of samples onto the grid.
		Each samples array gets its own channel.

                Args:
		    msg: the time domain data as a character array
		"""
		if not self[RUNNING_KEY]: return
		#check time elapsed
		if time.time() - self.frame_rate_ts < 1.0/self[FRAME_RATE_KEY]: return
		#convert to floating point numbers
		samples = numpy.fromstring(msg, numpy.float32)
		#extract the trigger offset
		self.trigger_offset = samples[-1]
		samples = samples[:-1]
		samps_per_ch = len(samples)/self.num_inputs
		self.sampleses = [samples[samps_per_ch*i:samps_per_ch*(i+1)] for i in range(self.num_inputs)]
		#handle samples
		self.handle_samples()
		self.frame_rate_ts = time.time()

	def set_auto_trigger_level(self, *args):
		"""
		Use the current trigger channel and samples to calculate the 50% level.
		"""
		if not self.sampleses: return
		samples = self.sampleses[self[TRIGGER_CHANNEL_KEY]]
		self[TRIGGER_LEVEL_KEY] = (numpy.max(samples)+numpy.min(samples))/2

	def handle_samples(self):
		"""
		Handle the cached samples from the scope input.
		Perform ac coupling, triggering, and auto ranging.
		"""
		if not self.sampleses: return
		sampleses = self.sampleses
		if self[XY_MODE_KEY]:
			self[DECIMATION_KEY] = 1
			x_samples = sampleses[self[X_CHANNEL_KEY]]
			y_samples = sampleses[self[Y_CHANNEL_KEY]]
			#autorange
			if self[AUTORANGE_KEY] and time.time() - self.autorange_ts > AUTORANGE_UPDATE_RATE:
				x_min, x_max = common.get_min_max(x_samples)
				y_min, y_max = common.get_min_max(y_samples)
				#adjust the x per div
				x_per_div = common.get_clean_num((x_max-x_min)/self[X_DIVS_KEY])
				if x_per_div != self[X_PER_DIV_KEY]: self[X_PER_DIV_KEY] = x_per_div; return
				#adjust the x offset
				x_off = x_per_div*round((x_max+x_min)/2/x_per_div)
				if x_off != self[X_OFF_KEY]: self[X_OFF_KEY] = x_off; return
				#adjust the y per div
				y_per_div = common.get_clean_num((y_max-y_min)/self[Y_DIVS_KEY])
				if y_per_div != self[Y_PER_DIV_KEY]: self[Y_PER_DIV_KEY] = y_per_div; return
				#adjust the y offset
				y_off = y_per_div*round((y_max+y_min)/2/y_per_div)
				if y_off != self[Y_OFF_KEY]: self[Y_OFF_KEY] = y_off; return
				self.autorange_ts = time.time()
			#plot xy channel
			self.plotter.set_waveform(
				channel='XY',
				samples=(x_samples, y_samples),
				color_spec=CHANNEL_COLOR_SPECS[0],
				marker=self[XY_MARKER_KEY],
			)
			#turn off each waveform
			for i, samples in enumerate(sampleses):
				self.plotter.clear_waveform(channel='Ch%d'%(i+1))
		else:
			#autorange
			if self[AUTORANGE_KEY] and time.time() - self.autorange_ts > AUTORANGE_UPDATE_RATE:
				bounds = [common.get_min_max(samples) for samples in sampleses]
				y_min = numpy.min([bound[0] for bound in bounds])
				y_max = numpy.max([bound[1] for bound in bounds])
				#adjust the y per div
				y_per_div = common.get_clean_num((y_max-y_min)/self[Y_DIVS_KEY])
				if y_per_div != self[Y_PER_DIV_KEY]: self[Y_PER_DIV_KEY] = y_per_div; return
				#adjust the y offset
				y_off = y_per_div*round((y_max+y_min)/2/y_per_div)
				if y_off != self[Y_OFF_KEY]: self[Y_OFF_KEY] = y_off; return
				self.autorange_ts = time.time()
			#number of samples to scale to the screen
			actual_rate = self.get_actual_rate()
			time_span = self[T_PER_DIV_KEY]*self[T_DIVS_KEY]
			num_samps = int(round(time_span*actual_rate))
			#handle the time offset
			t_off = self[T_FRAC_OFF_KEY]*(len(sampleses[0])/actual_rate - time_span)
			if t_off != self[T_OFF_KEY]: self[T_OFF_KEY] = t_off; return
			samps_off = int(round(actual_rate*self[T_OFF_KEY]))
			#adjust the decim so that we use about half the samps
			self[DECIMATION_KEY] = int(round(
					time_span*self[SAMPLE_RATE_KEY]/(0.5*len(sampleses[0]))
				)
			)
			#num samps too small, auto increment the time
			if num_samps < 2: self[T_PER_DIV_KEY] = common.get_clean_incr(self[T_PER_DIV_KEY])
			#num samps in bounds, plot each waveform
			elif num_samps <= len(sampleses[0]):
				for i, samples in enumerate(sampleses):
					#plot samples
					self.plotter.set_waveform(
						channel='Ch%d'%(i+1),
						samples=samples[samps_off:num_samps+samps_off],
						color_spec=CHANNEL_COLOR_SPECS[i],
						marker=self[common.index_key(MARKER_KEY, i)],
						trig_off=self.trigger_offset,
					)
			#turn XY channel off
			self.plotter.clear_waveform(channel='XY')
		#keep trigger level within range
		if self[TRIGGER_LEVEL_KEY] > self.get_y_max():
			self[TRIGGER_LEVEL_KEY] = self.get_y_max(); return
		if self[TRIGGER_LEVEL_KEY] < self.get_y_min():
			self[TRIGGER_LEVEL_KEY] = self.get_y_min(); return
		#disable the trigger channel
		if not self[TRIGGER_SHOW_KEY] or self[XY_MODE_KEY] or self[TRIGGER_MODE_KEY] == wxgui.TRIG_MODE_FREE:
			self.plotter.clear_waveform(channel='Trig')
		else: #show trigger channel
			trigger_level = self[TRIGGER_LEVEL_KEY]
			trigger_point = (len(self.sampleses[0])-1)/self.get_actual_rate()/2.0
			self.plotter.set_waveform(
				channel='Trig',
				samples=(
					[self.get_t_min(), trigger_point, trigger_point, trigger_point, trigger_point, self.get_t_max()],
					[trigger_level, trigger_level, self.get_y_max(), self.get_y_min(), trigger_level, trigger_level]
				),
				color_spec=TRIGGER_COLOR_SPEC,
			)
		#update the plotter
		self.plotter.update()

	def get_actual_rate(self): return 1.0*self[SAMPLE_RATE_KEY]/self[DECIMATION_KEY]
	def get_t_min(self): return self[T_OFF_KEY]
	def get_t_max(self): return self[T_PER_DIV_KEY]*self[T_DIVS_KEY] + self[T_OFF_KEY]
	def get_x_min(self): return -1*self[X_PER_DIV_KEY]*self[X_DIVS_KEY]/2.0 + self[X_OFF_KEY]
	def get_x_max(self): return self[X_PER_DIV_KEY]*self[X_DIVS_KEY]/2.0 + self[X_OFF_KEY]
	def get_y_min(self): return -1*self[Y_PER_DIV_KEY]*self[Y_DIVS_KEY]/2.0 + self[Y_OFF_KEY]
	def get_y_max(self): return self[Y_PER_DIV_KEY]*self[Y_DIVS_KEY]/2.0 + self[Y_OFF_KEY]

	def update_grid(self, *args):
		"""
		Update the grid to reflect the current settings:
		xy divisions, xy offset, xy mode setting
		"""
		if self[T_FRAC_OFF_KEY] < 0: self[T_FRAC_OFF_KEY] = 0; return
		if self[T_FRAC_OFF_KEY] > 1: self[T_FRAC_OFF_KEY] = 1; return
		if self[XY_MODE_KEY]:
			#update the x axis
			self.plotter.set_x_label('Ch%d'%(self[X_CHANNEL_KEY]+1))
			self.plotter.set_x_grid(self.get_x_min(), self.get_x_max(), self[X_PER_DIV_KEY])
			#update the y axis
			self.plotter.set_y_label('Ch%d'%(self[Y_CHANNEL_KEY]+1))
			self.plotter.set_y_grid(self.get_y_min(), self.get_y_max(), self[Y_PER_DIV_KEY])
		else:
			#update the t axis
			self.plotter.set_x_label('Time', 's')
			self.plotter.set_x_grid(self.get_t_min(), self.get_t_max(), self[T_PER_DIV_KEY], True)
			#update the y axis
			self.plotter.set_y_label(self[Y_AXIS_LABEL])
			self.plotter.set_y_grid(self.get_y_min(), self.get_y_max(), self[Y_PER_DIV_KEY])
		#redraw current sample
		self.handle_samples()

