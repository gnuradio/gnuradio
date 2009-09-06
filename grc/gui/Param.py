"""
Copyright 2007, 2008, 2009 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

import Utils
from Element import Element
import pygtk
pygtk.require('2.0')
import gtk

class InputParam(gtk.HBox):
	"""The base class for an input parameter inside the input parameters dialog."""

	def __init__(self, param, callback=None):
		gtk.HBox.__init__(self)
		self.param = param
		self._callback = callback
		self.label = gtk.Label('') #no label, markup is added by set_markup
		self.label.set_size_request(150, -1)
		self.pack_start(self.label, False)
		self.set_markup = lambda m: self.label.set_markup(m)
		self.tp = None
	def set_color(self, color): pass

	def update(self):
		"""
		Set the markup, color, and tooltip.
		"""
		#set the markup
		has_cb = \
			hasattr(self.param.get_parent(), 'get_callbacks') and \
			filter(lambda c: self.param.get_key() in c, self.param.get_parent()._callbacks)
		self.set_markup(Utils.parse_template(PARAM_LABEL_MARKUP_TMPL, param=self.param, has_cb=has_cb))
		#set the color
		self.set_color(self.param.get_color())
		#set the tooltip
		if self.tp: self.tp.set_tip(
			self.entry,
			Utils.parse_template(TIP_MARKUP_TMPL, param=self.param).strip(),
		)

	def _handle_changed(self, *args):
		"""
		Handle a gui change by setting the new param value,
		calling the callback (if applicable), and updating.
		"""
		#set the new value
		self.param.set_value(self.get_text())
		#call the callback
		if self._callback: self._callback()
		#self.update() #dont update here, parent will update

class EntryParam(InputParam):
	"""Provide an entry box for strings and numbers."""

	def __init__(self, *args, **kwargs):
		InputParam.__init__(self, *args, **kwargs)
		self.entry = input = gtk.Entry()
		input.set_text(self.param.get_value())
		input.connect('changed', self._handle_changed)
		self.pack_start(input, True)
		self.get_text = input.get_text
		#tool tip
		self.tp = gtk.Tooltips()
		self.tp.set_tip(self.entry, '')
		self.tp.enable()
	def set_color(self, color): self.entry.modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse(color))

class EnumParam(InputParam):
	"""Provide an entry box for Enum types with a drop down menu."""

	def __init__(self, *args, **kwargs):
		InputParam.__init__(self, *args, **kwargs)
		self._input = gtk.combo_box_new_text()
		for option in self.param.get_options(): self._input.append_text(option.get_name())
		self._input.set_active(self.param.get_option_keys().index(self.param.get_value()))
		self._input.connect('changed', self._handle_changed)
		self.pack_start(self._input, False)
	def get_text(self): return self.param.get_option_keys()[self._input.get_active()]

class EnumEntryParam(InputParam):
	"""Provide an entry box and drop down menu for Raw Enum types."""

	def __init__(self, *args, **kwargs):
		InputParam.__init__(self, *args, **kwargs)
		self._input = gtk.combo_box_entry_new_text()
		for option in self.param.get_options(): self._input.append_text(option.get_name())
		try: self._input.set_active(self.param.get_option_keys().index(self.param.get_value()))
		except:
			self._input.set_active(-1)
			self._input.get_child().set_text(self.param.get_value())
		self._input.connect('changed', self._handle_changed)
		self._input.get_child().connect('changed', self._handle_changed)
		self.pack_start(self._input, False)
	def get_text(self):
		if self._input.get_active() == -1: return self._input.get_child().get_text()
		return self.param.get_option_keys()[self._input.get_active()]
	def set_color(self, color):
		if self._input.get_active() == -1: #custom entry, use color
			self._input.get_child().modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse(color))
		else: #from enum, make white background
			self._input.get_child().modify_base(gtk.STATE_NORMAL, gtk.gdk.color_parse('#ffffff'))

PARAM_MARKUP_TMPL="""\
#set $foreground = $param.is_valid() and 'black' or 'red'
<span foreground="$foreground" font_desc="Sans 7.5"><b>$encode($param.get_name()): </b>$encode(repr($param))</span>"""

PARAM_LABEL_MARKUP_TMPL="""\
#set $foreground = $param.is_valid() and 'black' or 'red'
#set $underline = $has_cb and 'low' or 'none'
<span underline="$underline" foreground="$foreground" font_desc="Sans 9">$encode($param.get_name())</span>"""

TIP_MARKUP_TMPL="""\
Key: $param.get_key()
Type: $param.get_type()
#if $param.is_valid()
Value: $param.get_evaluated()
#elif len($param.get_error_messages()) == 1
Error: $(param.get_error_messages()[0])
#else
Error:
	#for $error_msg in $param.get_error_messages()
 * $error_msg
	#end for
#end if"""

class Param(Element):
	"""The graphical parameter."""

	def __init__(self): Element.__init__(self)

	def get_input_class(self):
		"""
		Get the graphical gtk class to represent this parameter.
		An enum requires and combo parameter.
		A non-enum with options gets a combined entry/combo parameter.
		All others get a standard entry parameter.
		@return gtk input class
		"""
		if self.is_enum(): return EnumParam
		if self.get_options(): return EnumEntryParam
		return EntryParam

	def get_layout(self):
		"""
		Create a layout based on the current markup.
		@return the pango layout
		"""
		layout = gtk.DrawingArea().create_pango_layout('')
		layout.set_markup(Utils.parse_template(PARAM_MARKUP_TMPL, param=self))
		return layout
