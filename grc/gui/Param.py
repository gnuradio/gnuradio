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

PARAM_MARKUP_TMPL="""\
#set $foreground = $param.is_valid() and 'black' or 'red'
#set $value = not $param.is_valid() and 'error' or repr($param)
<span foreground="$foreground" font_desc="Sans 7.5"><b>$encode($param.get_name()): </b>$encode($value)</span>"""

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
