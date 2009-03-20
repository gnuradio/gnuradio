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

#A macro to apply an index to a key
index_key = lambda key, i: "%s_%d"%(key, i+1)

def _register_access_method(destination, controller, key):
	"""
	Helper function for register access methods.
	This helper creates distinct set and get methods for each key
	and adds them to the destination object.
	"""
	def set(value): controller[key] = value
	setattr(destination, 'set_'+key, set)
	def get(): return controller[key]
	setattr(destination, 'get_'+key, get) 

def register_access_methods(destination, controller):
	"""
	Register setter and getter functions in the destination object for all keys in the controller.
	@param destination the object to get new setter and getter methods
	@param controller the pubsub controller
	"""
	for key in controller.keys(): _register_access_method(destination, controller, key)

##################################################
# Input Watcher Thread
##################################################
import threading

class input_watcher(threading.Thread):
	"""
	Input watcher thread runs forever.
	Read messages from the message queue.
	Forward messages to the message handler.
	"""
	def __init__ (self, msgq, controller, msg_key, arg1_key='', arg2_key=''):
		threading.Thread.__init__(self)
		self.setDaemon(1)
		self.msgq = msgq
		self._controller = controller
		self._msg_key = msg_key
		self._arg1_key = arg1_key
		self._arg2_key = arg2_key
		self.keep_running = True
		self.start()

	def run(self):
		while self.keep_running:
			msg = self.msgq.delete_head()
			if self._arg1_key: self._controller[self._arg1_key] = msg.arg1()
			if self._arg2_key: self._controller[self._arg2_key] = msg.arg2()
			self._controller[self._msg_key] = msg.to_string()

##################################################
# WX Shared Classes
##################################################
import math
import wx

EVT_DATA = wx.PyEventBinder(wx.NewEventType())
class DataEvent(wx.PyEvent):
	def __init__(self, data):
		wx.PyEvent.__init__(self, wx.NewId(), EVT_DATA.typeId)
		self.data = data

class LabelText(wx.StaticText):
	"""
	Label text to give the wx plots a uniform look.
	Get the default label text and set the font bold.
	"""
	def __init__(self, parent, label):
		wx.StaticText.__init__(self, parent, label=label)
		font = self.GetFont()
		font.SetWeight(wx.FONTWEIGHT_BOLD)
		self.SetFont(font)

class LabelBox(wx.BoxSizer):
	def __init__(self, parent, label, widget):
		wx.BoxSizer.__init__(self, wx.HORIZONTAL)
		self.Add(wx.StaticText(parent, label=' %s '%label), 1, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_LEFT)
		self.Add(widget, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_RIGHT)

class IncrDecrButtons(wx.BoxSizer):
	"""
	A horizontal box sizer with a increment and a decrement button.
	"""
	def __init__(self, parent, on_incr, on_decr):
		"""
		@param parent the parent window
		@param on_incr the event handler for increment
		@param on_decr the event handler for decrement
		"""
		wx.BoxSizer.__init__(self, wx.HORIZONTAL)
		self._incr_button = wx.Button(parent, label='+', style=wx.BU_EXACTFIT)
		self._incr_button.Bind(wx.EVT_BUTTON, on_incr)
		self.Add(self._incr_button, 0, wx.ALIGN_CENTER_VERTICAL)
		self._decr_button = wx.Button(parent, label=' - ', style=wx.BU_EXACTFIT)
		self._decr_button.Bind(wx.EVT_BUTTON, on_decr)
		self.Add(self._decr_button, 0, wx.ALIGN_CENTER_VERTICAL)

	def Disable(self): self.Enable(False)
	def Enable(self, enable=True):
		if enable:
			self._incr_button.Enable()
			self._decr_button.Enable()
		else:
			self._incr_button.Disable()
			self._decr_button.Disable()

class ToggleButtonController(wx.Button):
	def __init__(self, parent, controller, control_key, true_label, false_label):
		self._controller = controller
		self._control_key = control_key
		wx.Button.__init__(self, parent, style=wx.BU_EXACTFIT)
		self.Bind(wx.EVT_BUTTON, self._evt_button)
		controller.subscribe(control_key, lambda x: self.SetLabel(x and true_label or false_label))

	def _evt_button(self, e):
		self._controller[self._control_key] = not self._controller[self._control_key]

class CheckBoxController(wx.CheckBox):
	def __init__(self, parent, label, controller, control_key):
		self._controller = controller
		self._control_key = control_key
		wx.CheckBox.__init__(self, parent, style=wx.CHK_2STATE, label=label)
		self.Bind(wx.EVT_CHECKBOX, self._evt_checkbox)
		controller.subscribe(control_key, lambda x: self.SetValue(bool(x)))

	def _evt_checkbox(self, e):
		self._controller[self._control_key] = bool(e.IsChecked())

from gnuradio import eng_notation

class TextBoxController(wx.TextCtrl):
	def __init__(self, parent, controller, control_key, cast=float):
		self._controller = controller
		self._control_key = control_key
		self._cast = cast
		wx.TextCtrl.__init__(self, parent, style=wx.TE_PROCESS_ENTER)
		self.Bind(wx.EVT_TEXT_ENTER, self._evt_enter)
		controller.subscribe(control_key, lambda x: self.SetValue(eng_notation.num_to_str(x)))

	def _evt_enter(self, e):
		try: self._controller[self._control_key] = self._cast(eng_notation.str_to_num(self.GetValue()))
		except: self._controller[self._control_key] = self._controller[self._control_key]

class LogSliderController(wx.BoxSizer):
	"""
	Log slider controller with display label and slider.
	Gives logarithmic scaling to slider operation.
	"""
	def __init__(self, parent, prefix, min_exp, max_exp, slider_steps, controller, control_key, formatter=lambda x: ': %.6f'%x):
		self._prefix = prefix
		self._min_exp = min_exp
		self._max_exp = max_exp
		self._controller = controller
		self._control_key = control_key
		self._formatter = formatter
		wx.BoxSizer.__init__(self, wx.VERTICAL)
		self._label = wx.StaticText(parent, label=prefix + formatter(1/3.0))
		self.Add(self._label, 0, wx.EXPAND)
		self._slider = wx.Slider(parent, minValue=0, maxValue=slider_steps, style=wx.SL_HORIZONTAL)
		self.Add(self._slider, 0, wx.EXPAND)
		self._slider.Bind(wx.EVT_SLIDER, self._on_slider_event)
		controller.subscribe(control_key, self._on_controller_set)

	def _get_slope(self):
		return float(self._max_exp-self._min_exp)/self._slider.GetMax()

	def _on_slider_event(self, e):
		self._controller[self._control_key] = 10**(self._get_slope()*self._slider.GetValue() + self._min_exp)

	def _on_controller_set(self, value):
		self._label.SetLabel(self._prefix + self._formatter(value))
		slider_value = (math.log10(value)-self._min_exp)/self._get_slope()
		slider_value = min(max(self._slider.GetMin(), slider_value), self._slider.GetMax())
		if abs(slider_value - self._slider.GetValue()) > 1:
			self._slider.SetValue(slider_value)

	def Disable(self): self.Enable(False)
	def Enable(self, enable=True):
		if enable:
			self._slider.Enable()
			self._label.Enable()
		else:
			self._slider.Disable()
			self._label.Disable()

class DropDownController(wx.Choice):
	"""
	Drop down controller with label and chooser.
	Srop down selection from a set of choices.
	"""
	def __init__(self, parent, choices, controller, control_key, size=(-1, -1)):
		"""
		@param parent the parent window
		@param choices a list of tuples -> (label, value)
		@param controller the prop val controller
		@param control_key the prop key for this control
		"""
		self._controller = controller
		self._control_key = control_key
		self._choices = choices
		wx.Choice.__init__(self, parent, choices=[c[0] for c in choices], size=size)
		self.Bind(wx.EVT_CHOICE, self._on_chooser_event)
		controller.subscribe(control_key, self._on_controller_set)

	def _on_chooser_event(self, e):
		self._controller[self._control_key] = self._choices[self.GetSelection()][1]

	def _on_controller_set(self, value):
		#only set the chooser if the value is a possible choice
		for i, choice in enumerate(self._choices):
			if value == choice[1]: self.SetSelection(i)

##################################################
# Shared Functions
##################################################
import numpy
import math

def get_exp(num):
	"""
	Get the exponent of the number in base 10.
	@param num the floating point number
	@return the exponent as an integer
	"""
	if num == 0: return 0
	return int(math.floor(math.log10(abs(num))))

def get_clean_num(num):
	"""
	Get the closest clean number match to num with bases 1, 2, 5.
	@param num the number
	@return the closest number
	"""
	if num == 0: return 0
	sign = num > 0 and 1 or -1
	exp = get_exp(num)
	nums = numpy.array((1, 2, 5, 10))*(10**exp)
	return sign*nums[numpy.argmin(numpy.abs(nums - abs(num)))]

def get_clean_incr(num):
	"""
	Get the next higher clean number with bases 1, 2, 5.
	@param num the number
	@return the next higher number
	"""
	num = get_clean_num(num)
	exp = get_exp(num)
	coeff = int(round(num/10**exp))
	return {
		-5: -2,
		-2: -1,
		-1: -.5,
		1: 2,
		2: 5,
		5: 10,
	}[coeff]*(10**exp)

def get_clean_decr(num):
	"""
	Get the next lower clean number with bases 1, 2, 5.
	@param num the number
	@return the next lower number
	"""
	num = get_clean_num(num)
	exp = get_exp(num)
	coeff = int(round(num/10**exp))
	return {
		-5: -10,
		-2: -5,
		-1: -2,
		1: .5,
		2: 1,
		5: 2,
	}[coeff]*(10**exp)

def get_min_max(samples):
	"""
	Get the minimum and maximum bounds for an array of samples.
	@param samples the array of real values
	@return a tuple of min, max
	"""
	scale_factor = 3
	mean = numpy.average(samples)
	rms = numpy.max([scale_factor*((numpy.sum((samples-mean)**2)/len(samples))**.5), .1])
	min = mean - rms
	max = mean + rms
	return min, max
