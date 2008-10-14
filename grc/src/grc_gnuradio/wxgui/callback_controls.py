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

import wx
import sys

class LabelText(wx.StaticText):
	"""Label text class for uniform labels among all controls."""

	def __init__(self, window, label):
		wx.StaticText.__init__(self, window, -1, str(label))
		font = self.GetFont()
		font.SetWeight(wx.FONTWEIGHT_BOLD)
		self.SetFont(font)

class _control_base(wx.BoxSizer):
	"""Control base class"""

	def __init__(self, window, callback):
		self.window = window
		self.callback = callback
		wx.BoxSizer.__init__(self, wx.VERTICAL)

	def get_window(self): return self.window

	def call(self): return self.callback(self.get_value())

	def get_value(self): raise NotImplementedError

class _chooser_control_base(_control_base):
	"""House a drop down or radio buttons for variable control."""

	def __init__(self, window, callback, label='Label', index=0, choices=[0], labels=[]):
		"""
		Chooser contructor.
		Create the slider, text box, and label.
		@param window the wx parent window
		@param callback call the callback on changes
		@param label the label title
		@param index the default choice index
		@param choices a list of choices
		@param labels the choice labels or empty list
		"""
		#initialize
		_control_base.__init__(self, window, callback)
		label_text = LabelText(self.get_window(), label)
		self.Add(label_text, 0, wx.ALIGN_CENTER)
		self.index = index
		self.choices = choices
		self.labels = map(str, labels or choices)
		self._init()

  	def _handle_changed(self, event=None):
		"""
		A change is detected. Call the callback.
		"""
		try: self.call()
		except Exception, e: print >> sys.stderr, 'Error in exec callback from handle changed.\n', e

	def get_value(self):
		"""
		Update the chooser.
		@return one of the possible choices
		"""
		self._update()
		return self.choices[self.index]

##############################################################################################
# Button Control
##############################################################################################
class button_control(_chooser_control_base):
	"""House a button for variable control."""

	def _init(self):
		self.button = wx.Button(self.get_window(), -1, self.labels[self.index])
		self.button.Bind(wx.EVT_BUTTON, self._handle_changed)
		self.Add(self.button, 0, wx.ALIGN_CENTER)

	def _update(self):
		self.index = (self.index + 1)%len(self.choices) #circularly increment index
		self.button.SetLabel(self.labels[self.index])

##############################################################################################
# Drop Down Control
##############################################################################################
class drop_down_control(_chooser_control_base):
	"""House a drop down for variable control."""

	def _init(self):
		self.drop_down = wx.Choice(self.get_window(), -1, choices=self.labels)
		self.Add(self.drop_down, 0, wx.ALIGN_CENTER)
		self.drop_down.Bind(wx.EVT_CHOICE, self._handle_changed)
		self.drop_down.SetSelection(self.index)

	def _update(self):
		self.index = self.drop_down.GetSelection()

##############################################################################################
# Radio Buttons Control
##############################################################################################
class _radio_buttons_control_base(_chooser_control_base):
	"""House radio buttons for variable control."""

	def _init(self):
		#create box for radio buttons
		radio_box = wx.BoxSizer(self.radio_box_orientation)
		panel = wx.Panel(self.get_window(), -1)
		panel.SetSizer(radio_box)
		self.Add(panel, 0, wx.ALIGN_CENTER)
		#create radio buttons
		self.radio_buttons = list()
		for label in self.labels:
			radio_button = wx.RadioButton(panel, -1, label)
			radio_button.SetValue(False)
			self.radio_buttons.append(radio_button)
			radio_box.Add(radio_button, 0, self.radio_button_align)
			radio_button.Bind(wx.EVT_RADIOBUTTON, self._handle_changed)
		#set one radio button active
		self.radio_buttons[self.index].SetValue(True)

	def _update(self):
		selected_radio_button = filter(lambda rb: rb.GetValue(), self.radio_buttons)[0]
		self.index = self.radio_buttons.index(selected_radio_button)

class radio_buttons_horizontal_control(_radio_buttons_control_base):
	radio_box_orientation = wx.HORIZONTAL
	radio_button_align = wx.ALIGN_CENTER
class radio_buttons_vertical_control(_radio_buttons_control_base):
	radio_box_orientation = wx.VERTICAL
	radio_button_align = wx.ALIGN_LEFT

##############################################################################################
# Slider Control
##############################################################################################
class _slider_control_base(_control_base):
	"""House a Slider and a Text Box for variable control."""

	def __init__(self, window, callback, label='Label', value=50, min=0, max=100, num_steps=100, slider_length=200):
		"""
		Slider contructor.
		Create the slider, text box, and label.
		@param window the wx parent window
		@param callback call the callback on changes
		@param label the label title
		@param value the default value
		@param min the min
		@param max the max
		@param num_steps the number of steps
		@param slider_length the length of the slider bar in pixels
		"""
		#initialize
		_control_base.__init__(self, window, callback)
		self.min = float(min)
		self.max = float(max)
		self.num_steps = int(num_steps)
		self.slider_length = slider_length
		#create gui elements
		label_text_sizer = wx.BoxSizer(self.label_text_orientation) #label and text box container
		label_text = LabelText(self.get_window(), '%s: '%str(label))
		self.text_box = text_box = wx.TextCtrl(self.get_window(), -1, str(value), style=wx.TE_PROCESS_ENTER)
		text_box.Bind(wx.EVT_TEXT_ENTER, self._handle_enter) #bind this special enter hotkey event
		for obj in (label_text, text_box): #fill the container with label and text entry box
			label_text_sizer.Add(obj, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL)
		self.Add(label_text_sizer, 0, wx.ALIGN_CENTER)
		#make the slider
		self.slider = slider = wx.Slider(self.get_window(), -1, size=wx.Size(*self.get_slider_size()), style=self.slider_style)
		try: slider.SetRange(0, num_steps)
		except Exception, e:
			print >> sys.stderr, 'Error in set slider range: "%s".'%e
			sys.exit(-1)
		slider.Bind(wx.EVT_SCROLL, self._handle_scroll) #bind the scrolling event
		self.Add(slider, 0, wx.ALIGN_CENTER)
		#init slider and text box
		self._value = value
		self._set_slider_value(self._value) #sets the slider's value
		self.text_box.SetValue(str(self._value))

	def get_value(self):
		"""
		Get the current set value.
		@return the value (float)
		"""
		return self._value

	def _set_slider_value(self, real_value):
		"""
		Translate the real numerical value into a slider value and,
		write the value to the slider.
		@param real_value the numeric value the slider should represent
		"""
		slider_value = (float(real_value) - self.min)*self.num_steps/(self.max - self.min)
		self.slider.SetValue(slider_value)

	def _handle_scroll(self, event=None):
		"""
		A scroll event is detected. Read the slider, call the callback.
		"""
		slider_value = self.slider.GetValue()
		new_value = slider_value*(self.max - self.min)/self.num_steps + self.min
		self.text_box.SetValue(str(new_value))
		self._value = new_value
		try: self.call()
		except Exception, e: print >> sys.stderr, 'Error in exec callback from handle scroll.\n', e

	def _handle_enter(self, event=None):
		"""
		An enter key was pressed. Read the text box, call the callback.
		"""
		new_value = float(self.text_box.GetValue())
		self._set_slider_value(new_value)
		self._value = new_value
		try: self.call()
		except Exception, e: print >> sys.stderr, 'Error in exec callback from handle enter.\n', e

class slider_horizontal_control(_slider_control_base):
	label_text_orientation = wx.HORIZONTAL
	slider_style = wx.SL_HORIZONTAL
	def get_slider_size(self): return self.slider_length, 20
class slider_vertical_control(_slider_control_base):
	label_text_orientation = wx.VERTICAL
	slider_style = wx.SL_VERTICAL
	def get_slider_size(self): return 20, self.slider_length

##############################################################################################
# Text Box Control
##############################################################################################
class text_box_control(_control_base):
	"""House a Text Box for variable control."""

	def __init__(self, window, callback, label='Label', value=50):
		"""
		Text box contructor.
		Create the text box, and label.
		@param window the wx parent window
		@param callback call the callback on changes
		@param label the label title
		@param value the default value
		"""
		#initialize
		_control_base.__init__(self, window, callback)
		#create gui elements
		label_text_sizer = wx.BoxSizer(wx.HORIZONTAL) #label and text box container
		label_text = LabelText(self.get_window(), '%s: '%str(label))
		self.text_box = text_box = wx.TextCtrl(self.get_window(), -1, str(value), style=wx.TE_PROCESS_ENTER)
		text_box.Bind(wx.EVT_TEXT_ENTER, self._handle_enter) #bind this special enter hotkey event
		for obj in (label_text, text_box): #fill the container with label and text entry box
			label_text_sizer.Add(obj, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_CENTER_VERTICAL)
		self.Add(label_text_sizer, 0, wx.ALIGN_CENTER)
		#set the value, detect string mode
		self._string_mode = isinstance(value, str)
		self.text_box.SetValue(str(value))

	def get_value(self):
		"""
		Get the current set value.
		@return the value (float)
		"""
		return self._value

	def _handle_enter(self, event=None):
		"""
		An enter key was pressed. Read the text box, call the callback.
		If the text cannot be evaluated, do not try callback.
		Do not evaluate the text box value in string mode.
		"""
		if self._string_mode: self._value = self.text_box.GetValue()
		else:
			try: self._value = eval(self.text_box.GetValue())
			except Exception, e:
				print >> sys.stderr, 'Error in evaluate value from handle enter.\n', e
				return
		try: self.call()
		except Exception, e: print >> sys.stderr, 'Error in exec callback from handle enter.\n', e
