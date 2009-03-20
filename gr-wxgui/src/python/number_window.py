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

##################################################
# Constants
##################################################
NEG_INF = float('-inf')
SLIDER_STEPS = 100
AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP = -3, 0
DEFAULT_NUMBER_RATE = gr.prefs().get_long('wxgui', 'number_rate', 5)
DEFAULT_WIN_SIZE = (300, 300)
DEFAULT_GAUGE_RANGE = 1000

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
		wx.Panel.__init__(self, parent, style=wx.SUNKEN_BORDER)
		control_box = wx.BoxSizer(wx.VERTICAL)
		#checkboxes for average and peak hold
		control_box.AddStretchSpacer()
		control_box.Add(common.LabelText(self, 'Options'), 0, wx.ALIGN_CENTER)
		self.peak_hold_check_box = common.CheckBoxController(self, 'Peak Hold', parent, PEAK_HOLD_KEY)
		control_box.Add(self.peak_hold_check_box, 0, wx.EXPAND)
		self.average_check_box = common.CheckBoxController(self, 'Average', parent, AVERAGE_KEY)
		control_box.Add(self.average_check_box, 0, wx.EXPAND)
		control_box.AddSpacer(2)
		self.avg_alpha_slider = common.LogSliderController(
			self, 'Avg Alpha',
			AVG_ALPHA_MIN_EXP, AVG_ALPHA_MAX_EXP, SLIDER_STEPS,
			parent, AVG_ALPHA_KEY,
			formatter=lambda x: ': %.4f'%x,
		)
		parent.subscribe(AVERAGE_KEY, self.avg_alpha_slider.Enable)
		control_box.Add(self.avg_alpha_slider, 0, wx.EXPAND)
		#run/stop
		control_box.AddStretchSpacer()
		self.run_button = common.ToggleButtonController(self, parent, RUNNING_KEY, 'Stop', 'Run')
		control_box.Add(self.run_button, 0, wx.EXPAND)
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
		self.minval = minval
		self.maxval = maxval
		self.decimal_places = decimal_places
		#proxy the keys
		self.proxy(MSG_KEY, controller, msg_key)
		self.proxy(AVERAGE_KEY, controller, average_key)
		self.proxy(AVG_ALPHA_KEY, controller, avg_alpha_key)
		self.proxy(SAMPLE_RATE_KEY, controller, sample_rate_key)
		#setup the box with display and controls
		self.control_panel = control_panel(self)
		main_box = wx.BoxSizer(wx.HORIZONTAL)
		sizer = wx.BoxSizer(wx.VERTICAL)
		main_box.Add(sizer, 1, wx.EXPAND)
		main_box.Add(self.control_panel, 0, wx.EXPAND)
		sizer.Add(common.LabelText(self, title), 1, wx.ALIGN_CENTER)
		self.text = wx.StaticText(self, size=(size[0], -1))
		sizer.Add(self.text, 1, wx.EXPAND)
		self.gauge_real = wx.Gauge(self, range=DEFAULT_GAUGE_RANGE, style=wx.GA_HORIZONTAL)
		self.gauge_imag = wx.Gauge(self, range=DEFAULT_GAUGE_RANGE, style=wx.GA_HORIZONTAL)
		#hide/show gauges
		self.show_gauges(show_gauge)
		sizer.Add(self.gauge_real, 1, wx.EXPAND)
		sizer.Add(self.gauge_imag, 1, wx.EXPAND)
		self.SetSizerAndFit(main_box)
		#initialize values
		self[PEAK_HOLD_KEY] = peak_hold
		self[RUNNING_KEY] = True
		self[AVERAGE_KEY] = self[AVERAGE_KEY]
		self[AVG_ALPHA_KEY] = self[AVG_ALPHA_KEY]
		#register events
		self.subscribe(MSG_KEY, self.handle_msg)
		self.Bind(common.EVT_DATA, self.update)

	def show_gauges(self, show_gauge):
		"""
		Show or hide the gauges.
		If this is real, never show the imaginary gauge.
		@param show_gauge true to show
		"""
		if show_gauge: self.gauge_real.Show()
		else: self.gauge_real.Hide()
		if show_gauge and not self.real: self.gauge_imag.Show()
		else: self.gauge_imag.Hide()

	def handle_msg(self, msg):
		"""
		Post this message into a data event.
		Allow wx to handle the event to avoid threading issues.
		@param msg the incoming numbersink data
		"""
		wx.PostEvent(self, common.DataEvent(msg))

	def update(self, event):
		"""
		Handle a message from the message queue.
		Convert the string based message into a float or complex.
		If more than one number was read, only take the last number.
		Perform peak hold operations, set the gauges and display.
		@param event event.data is the number sample as a character array
		"""
		if not self[RUNNING_KEY]: return
		#set gauge
		def set_gauge_value(gauge, value):
			gauge_val = DEFAULT_GAUGE_RANGE*(value-self.minval)/(self.maxval-self.minval)
			gauge_val = max(0, gauge_val) #clip
			gauge_val = min(DEFAULT_GAUGE_RANGE, gauge_val) #clip
			gauge.SetValue(gauge_val)
		format_string = "%%.%df"%self.decimal_places
		if self.real:
			sample = numpy.fromstring(event.data, numpy.float32)[-1]
			if self[PEAK_HOLD_KEY]: sample = self.peak_val_real = max(self.peak_val_real, sample)
			label_text = "%s %s"%(format_string%sample, self.units)
			set_gauge_value(self.gauge_real, sample)
		else:
			sample = numpy.fromstring(event.data, numpy.complex64)[-1]
			if self[PEAK_HOLD_KEY]:
				self.peak_val_real = max(self.peak_val_real, sample.real)
				self.peak_val_imag = max(self.peak_val_imag, sample.imag)
				sample = self.peak_val_real + self.peak_val_imag*1j
			label_text = "%s + %sj %s"%(format_string%sample.real, format_string%sample.imag, self.units)
			set_gauge_value(self.gauge_real, sample.real)
			set_gauge_value(self.gauge_imag, sample.imag)
		#set label text
		self.text.SetLabel(label_text)
		#clear peak hold
		if not self[PEAK_HOLD_KEY]:
			self.peak_val_real = NEG_INF
			self.peak_val_imag = NEG_INF
