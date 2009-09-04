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

	def __init__(self, param, _handle_changed):
		gtk.HBox.__init__(self)
		self.param = param
		self._handle_changed = _handle_changed
		self.label = gtk.Label('') #no label, markup is added by set_markup
		self.label.set_size_request(150, -1)
		self.pack_start(self.label, False)
		self.set_markup = lambda m: self.label.set_markup(m)
		self.tp = None
	def set_color(self, color): pass

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

	def update(self):
		"""
		Called when an external change occurs.
		Update the graphical input by calling the change handler.
		"""
		if hasattr(self, '_input'): self._handle_changed()

	def get_input_object(self, callback=None):
		"""
		Get the graphical gtk object to represent this parameter.
		Create the input object with this data type and the handle changed method.
		@param callback a function of one argument(this param) to be called from the change handler
		@return gtk input object
		"""
		self._callback = callback
		self._input = self.get_input_class()(self, self._handle_changed)
		if not self._callback: self.update()
		return self._input

	def _handle_changed(self, widget=None):
		"""
		When the input changes, write the inputs to the data type.
		Finish by calling the exteral callback.
		"""
		self.set_value(self._input.get_text())
		self.validate()
		#is param is involved in a callback? #FIXME: messy
		has_cb = \
			hasattr(self.get_parent(), 'get_callbacks') and \
			filter(lambda c: self.get_key() in c, self.get_parent()._callbacks)
		self._input.set_markup(Utils.parse_template(PARAM_LABEL_MARKUP_TMPL, param=self, has_cb=has_cb))
		#hide/show
		if self.get_hide() == 'all': self._input.hide_all()
		else: self._input.show_all()
		#set the color
		self._input.set_color(self.get_color())
		#set the tooltip
		if self._input.tp: self._input.tp.set_tip(
			self._input.entry,
			Utils.parse_template(TIP_MARKUP_TMPL, param=self).strip(),
		)
		#execute the external callback
		if self._callback: self._callback(self)

	def get_layout(self):
		"""
		Create a layout based on the current markup.
		@return the pango layout
		"""
		layout = gtk.DrawingArea().create_pango_layout('')
		layout.set_markup(Utils.parse_template(PARAM_MARKUP_TMPL, param=self))
		return layout
