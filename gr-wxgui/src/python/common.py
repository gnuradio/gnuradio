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

import threading
import numpy
import math
import wx

class prop_setter(object):
	def _register_set_prop(self, controller, control_key, *args):
		def set_method(value): controller[control_key] = value
		if args: set_method(args[0])
		setattr(self, 'set_%s'%control_key, set_method)

##################################################
# Custom Data Event
##################################################
EVT_DATA = wx.PyEventBinder(wx.NewEventType())
class DataEvent(wx.PyEvent):
	def __init__(self, data):
		wx.PyEvent.__init__(self, wx.NewId(), EVT_DATA.typeId)
		self.data = data

##################################################
# Input Watcher Thread
##################################################
class input_watcher(threading.Thread):
	"""
	Input watcher thread runs forever.
	Read messages from the message queue.
	Forward messages to the message handler.
	"""
	def __init__ (self, msgq, handle_msg):
		threading.Thread.__init__(self)
		self.setDaemon(1)
		self.msgq = msgq
		self._handle_msg = handle_msg
		self.keep_running = True
		self.start()

	def run(self):
		while self.keep_running: self._handle_msg(self.msgq.delete_head().to_string())

##################################################
# WX Shared Classes
##################################################
class LabelText(wx.StaticText):
	"""
	Label text to give the wx plots a uniform look.
	Get the default label text and set the font bold.
	"""
	def __init__(self, parent, label):
		wx.StaticText.__init__(self, parent, -1, label)
		font = self.GetFont()
		font.SetWeight(wx.FONTWEIGHT_BOLD)
		self.SetFont(font)

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
		self._incr_button = wx.Button(parent, -1, '+', style=wx.BU_EXACTFIT)
		self._incr_button.Bind(wx.EVT_BUTTON, on_incr)
		self.Add(self._incr_button, 0, wx.ALIGN_CENTER_VERTICAL)
		self._decr_button = wx.Button(parent, -1, ' - ', style=wx.BU_EXACTFIT)
		self._decr_button.Bind(wx.EVT_BUTTON, on_decr)
		self.Add(self._decr_button, 0, wx.ALIGN_CENTER_VERTICAL)

	def Disable(self, disable=True): self.Enable(not disable)
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
		wx.Button.__init__(self, parent, -1, '', style=wx.BU_EXACTFIT)
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

class LogSliderController(wx.BoxSizer):
	"""
	Log slider controller with display label and slider.
	Gives logarithmic scaling to slider operation.
	"""
	def __init__(self, parent, label, min_exp, max_exp, slider_steps, controller, control_key, formatter=lambda x: ': %.6f'%x):
		wx.BoxSizer.__init__(self, wx.VERTICAL)
		self._label = wx.StaticText(parent, -1, label + formatter(1/3.0))
		self.Add(self._label, 0, wx.EXPAND)
		self._slider = wx.Slider(parent, -1, 0, 0, slider_steps, style=wx.SL_HORIZONTAL)
		self.Add(self._slider, 0, wx.EXPAND)
		def _on_slider_event(event):
			controller[control_key] = \
			10**(float(max_exp-min_exp)*self._slider.GetValue()/slider_steps + min_exp)
		self._slider.Bind(wx.EVT_SLIDER, _on_slider_event)
		def _on_controller_set(value):
			self._label.SetLabel(label + formatter(value))
			slider_value = slider_steps*(math.log10(value)-min_exp)/(max_exp-min_exp)
			slider_value = min(max(0, slider_value), slider_steps)
			if abs(slider_value - self._slider.GetValue()) > 1:
				self._slider.SetValue(slider_value)
		controller.subscribe(control_key, _on_controller_set)

	def Disable(self, disable=True): self.Enable(not disable)
	def Enable(self, enable=True):
		if enable:
			self._slider.Enable()
			self._label.Enable()
		else:
			self._slider.Disable()
			self._label.Disable()

class DropDownController(wx.BoxSizer):
	"""
	Drop down controller with label and chooser.
	Srop down selection from a set of choices.
	"""
	def __init__(self, parent, label, choices, controller, control_key, size=(-1, -1)):
		"""
		@param parent the parent window
		@param label the label for the drop down
		@param choices a list of tuples -> (label, value)
		@param controller the prop val controller
		@param control_key the prop key for this control
		"""
		wx.BoxSizer.__init__(self, wx.HORIZONTAL)
		self._label = wx.StaticText(parent, -1, ' %s '%label)
		self.Add(self._label, 1, wx.ALIGN_CENTER_VERTICAL)
		self._chooser = wx.Choice(parent, -1, choices=[c[0] for c in choices], size=size)
		def _on_chooser_event(event):
			controller[control_key] = choices[self._chooser.GetSelection()][1]
		self._chooser.Bind(wx.EVT_CHOICE, _on_chooser_event)
		self.Add(self._chooser, 0, wx.ALIGN_CENTER_VERTICAL)
		def _on_controller_set(value):
			#only set the chooser if the value is a possible choice
			for i, choice in enumerate(choices):
				if value == choice[1]: self._chooser.SetSelection(i)
		controller.subscribe(control_key, _on_controller_set)

	def Disable(self, disable=True): self.Enable(not disable)
	def Enable(self, enable=True):
		if enable:
			self._chooser.Enable()
			self._label.Enable()
		else:
			self._chooser.Disable()
			self._label.Disable()

##################################################
# Shared Functions
##################################################
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
	if num > 0: sign = 1
	else: sign = -1
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
	rms = scale_factor*((numpy.sum((samples-mean)**2)/len(samples))**.5)
	min = mean - rms
	max = mean + rms
	return min, max

def get_si_components(num):
	"""
	Get the SI units for the number.
	Extract the coeff and exponent of the number.
	The exponent will be a multiple of 3.
	@param num the floating point number
	@return the tuple coeff, exp, prefix
	"""
	exp = get_exp(num)
	exp -= exp%3
	exp = min(max(exp, -24), 24) #bounds on SI table below
	prefix = {
		24: 'Y', 21: 'Z',
		18: 'E', 15: 'P',
		12: 'T', 9: 'G',
		6: 'M', 3: 'K',
		0: '',
		-3: 'm', -6: 'u',
		-9: 'n', -12: 'p',
		-15: 'f', -18: 'a',
		-21: 'z', -24: 'y',
	}[exp]
	coeff = num/10**exp
	return coeff, exp, prefix

def label_format(num):
	"""
	Format a floating point number into a presentable string.
	If the number has an small enough exponent, use regular decimal.
	Otherwise, format the number with floating point notation.
	Exponents are normalized to multiples of 3.
	In the case where the exponent was found to be -3,
	it is best to display this as a regular decimal, with a 0 to the left.
	@param num the number to format
	@return a label string
	"""
	coeff, exp, prefix = get_si_components(num)
	if -3 <= exp < 3: return '%g'%num
	return '%se%d'%('%.3g'%coeff, exp)

if __name__ == '__main__':
	import random
	for i in range(-25, 25):
		num = random.random()*10**i
		print num, ':', get_si_components(num)
