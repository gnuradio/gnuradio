"""
Copyright 2007 Free Software Foundation, Inc.
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
import pango
from Constants import PARAM_LABEL_FONT, PARAM_FONT

class Param(Element):
	"""The graphical parameter."""

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
		value = self._input.get_text()
		if self.is_enum(): value = self.get_option_keys()[int(value)]
		self.set_value(value)
		#set the markup on the label, red for errors in corresponding data type.
		name = '<span font_desc="%s">%s</span>'%(
			PARAM_LABEL_FONT,
			Utils.xml_encode(self.get_name()),
		)
		#special markups if param is involved in a callback
		if hasattr(self.get_parent(), 'get_callbacks') and \
			filter(lambda c: self.get_key() in c, self.get_parent()._callbacks):
			name = '<span underline="low">%s</span>'%name
		if not self.is_valid():
			self._input.set_markup('<span foreground="red">%s</span>'%name)
			tip = 'Error: ' + ' '.join(self.get_error_messages())
		else:
			self._input.set_markup(name)
			tip = 'Value: %s'%str(self.evaluate())
		#hide/show
		if self.get_hide() == 'all': self._input.hide_all()
		else: self._input.show_all()
		#set the color
		self._input.set_color(self.get_color())
		#set the tooltip
		if self._input.tp: self._input.tp.set_tip(
			self._input.entry,
			'Key: %s\nType: %s\n%s'%(self.get_key(), self.get_type(), tip),
		)
		#execute the external callback
		if self._callback: self._callback(self)

	def get_markup(self):
		"""
		Create a markup to display the param as a label on the block.
		If the param is valid, use the param's repr representation.
		Otherwise, create a markup for error.
		@return pango markup string
		"""
		if self.is_valid():
			return '<b>%s:</b> %s'%(Utils.xml_encode(self.get_name()), Utils.xml_encode(repr(self)))
		else:
			return '<span foreground="red"><b>%s:</b> error</span>'%Utils.xml_encode(self.get_name())

	def get_layout(self):
		"""
		Create a layout based on the current markup.
		@return the pango layout
		"""
		layout = gtk.DrawingArea().create_pango_layout('')
		layout.set_markup(self.get_markup())
		desc = pango.FontDescription(PARAM_FONT)
		layout.set_font_description(desc)
		return layout
