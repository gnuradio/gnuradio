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
import common
import numpy
import wx
import pubsub
from constants import *
from gnuradio import gr #for gr.prefs
import forms

##################################################
# Constants
##################################################
NEG_INF = float('-inf')
SLIDER_STEPS = 100
AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP = -3, 0
DEFAULT_NUMBER_RATE = gr.prefs().get_long('wxgui', 'number_rate', 5)
DEFAULT_WIN_SIZE = (300, 300)
DEFAULT_GAUGE_RANGE = 1000
VALUE_REPR_KEY = 'value_repr'
VALUE_REAL_KEY = 'value_real'
VALUE_IMAG_KEY = 'value_imag'

##################################################
# Number window control panel
##################################################
class control_panel(wx.Panel):
	"""
	A control panel with wx widgits to control the averaging.
	"""

	def __init__(self, parent):
		"""
		Create a new control panel.
		@param parent the wx parent window
		"""
		self.parent = parent
		wx.Panel.__init__(self, parent)
		parent[SHOW_CONTROL_PANEL_KEY] = True
		parent.subscribe(SHOW_CONTROL_PANEL_KEY, self.Show)
		control_box = wx.BoxSizer(wx.VERTICAL)
		#checkboxes for average and peak hold
		control_box.AddStretchSpacer()
		options_box = forms.static_box_sizer(
			parent=self, sizer=control_box, label='Options',
			bold=True, orient=wx.VERTICAL,
		)
		forms.check_box(
			sizer=options_box, parent=self, label='Peak Hold',
			ps=parent, key=PEAK_HOLD_KEY,
		)
		forms.check_box(
			sizer=options_box, parent=self, label='Average',
			ps=parent, key=AVERAGE_KEY,
		)
		#static text and slider for averaging
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
		#run/stop
		control_box.AddStretchSpacer()
		forms.toggle_button(
			sizer=control_box, parent=self,
			true_label='Stop', false_label='Run',
			ps=parent, key=RUNNING_KEY,
		)
		#set sizer
		self.SetSizerAndFit(control_box)

##################################################
# Numbersink window with label and gauges
##################################################
class number_window(wx.Panel, pubsub.pubsub):
	def __init__(
		self,
		parent,
		controller,
		size,
		title,
		units,
		show_gauge,
		real,
		minval,
		maxval,
		decimal_places,
		average_key,
		avg_alpha_key,
		peak_hold,
		msg_key,
		sample_rate_key,
	):
		pubsub.pubsub.__init__(self)
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		#setup
		self.peak_val_real = NEG_INF
		self.peak_val_imag = NEG_INF
		self.real = real
		self.units = units
		self.decimal_places = decimal_places
		#proxy the keys
		self.proxy(MSG_KEY, controller, msg_key)
		self.proxy(AVERAGE_KEY, controller, average_key)
		self.proxy(AVG_ALPHA_KEY, controller, avg_alpha_key)
		self.proxy(SAMPLE_RATE_KEY, controller, sample_rate_key)
		#initialize values
		self[PEAK_HOLD_KEY] = peak_hold
		self[RUNNING_KEY] = True
		self[VALUE_REAL_KEY] = minval
		self[VALUE_IMAG_KEY] = minval
		#setup the box with display and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		sizer = forms.static_box_sizer(
			parent=self, sizer=main_box, label=title,
			bold=True, orient=wx.VERTICAL, proportion=1,
		)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		sizer.AddStretchSpacer()
		forms.static_text(
			parent=self, sizer=sizer,
			ps=self, key=VALUE_REPR_KEY, width=size[0],
			converter=forms.str_converter(),
		)
		sizer.AddStretchSpacer()
		self.gauge_real = forms.gauge(
			parent=self, sizer=sizer, style=wx.GA_HORIZONTAL,
			ps=self, key=VALUE_REAL_KEY, length=size[0],
			minimum=minval, maximum=maxval, num_steps=DEFAULT_GAUGE_RANGE,
		)
		self.gauge_imag = forms.gauge(
			parent=self, sizer=sizer, style=wx.GA_HORIZONTAL,
			ps=self, key=VALUE_IMAG_KEY, length=size[0],
			minimum=minval, maximum=maxval, num_steps=DEFAULT_GAUGE_RANGE,
		)
		#hide/show gauges
		self.show_gauges(show_gauge)
		self.SetSizerAndFit(main_box)
		#register events
		self.subscribe(MSG_KEY, self.handle_msg)

	def show_gauges(self, show_gauge):
		"""
		Show or hide the gauges.
		If this is real, never show the imaginary gauge.
		@param show_gauge true to show
		"""
		self.gauge_real.ShowItems(show_gauge)
		self.gauge_imag.ShowItems(show_gauge and not self.real)

	def handle_msg(self, msg):
		"""
		Handle a message from the message queue.
		Convert the string based message into a float or complex.
		If more than one number was read, only take the last number.
		Perform peak hold operations, set the gauges and display.
		@param event event.data is the number sample as a character array
		"""
		if not self[RUNNING_KEY]: return
		format_string = "%%.%df"%self.decimal_places
		if self.real:
			sample = numpy.fromstring(msg, numpy.float32)[-1]
			if self[PEAK_HOLD_KEY]: sample = self.peak_val_real = max(self.peak_val_real, sample)
			label_text = "%s %s"%(format_string%sample, self.units)
			self[VALUE_REAL_KEY] = sample
		else:
			sample = numpy.fromstring(msg, numpy.complex64)[-1]
			if self[PEAK_HOLD_KEY]:
				self.peak_val_real = max(self.peak_val_real, sample.real)
				self.peak_val_imag = max(self.peak_val_imag, sample.imag)
				sample = self.peak_val_real + self.peak_val_imag*1j
			label_text = "%s + %sj %s"%(format_string%sample.real, format_string%sample.imag, self.units)
			self[VALUE_REAL_KEY] = sample.real
			self[VALUE_IMAG_KEY] = sample.imag
		#set label text
		self[VALUE_REPR_KEY] = label_text
		#clear peak hold
		if not self[PEAK_HOLD_KEY]:
			self.peak_val_real = NEG_INF
			self.peak_val_imag = NEG_INF
