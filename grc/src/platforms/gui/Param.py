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
		Create a markup to display the Param as a label on the SignalBlock.
		If the data type is an Enum type, use the cname of the Enum's current choice.
		Otherwise, use parsed the data type and use its string representation.
		If the data type is not valid, use a red foreground color.
		@return pango markup string
		"""
		###########################################################################
		# display logic for numbers
		###########################################################################
		def float_to_str(var):
			if var-int(var) == 0: return '%d'%int(var)
			if var*10-int(var*10) == 0: return '%.1f'%var
			if var*100-int(var*100) == 0: return '%.2f'%var
			if var*1000-int(var*1000) == 0: return '%.3f'%var
			else: return '%.3g'%var
		def to_str(var):
			if isinstance(var, str): return var
			elif isinstance(var, complex):
				if var == 0: return '0' #value is zero
				elif var.imag == 0: return '%s'%float_to_str(var.real) #value is real
				elif var.real == 0: return '%sj'%float_to_str(var.imag) #value is imaginary
				elif var.imag < 0: return '%s-%sj'%(float_to_str(var.real), float_to_str(abs(var.imag)))
				else: return '%s+%sj'%(float_to_str(var.real), float_to_str(var.imag))
			elif isinstance(var, float): return float_to_str(var)
			elif isinstance(var, int): return '%d'%var
			else: return str(var)
		###########################################################################
		if self.is_valid():
			data = self.evaluate()
			t = self.get_type()
			if self.is_enum():
				dt_str = self.get_option(self.get_value()).get_name()
			elif isinstance(data, (list, tuple, set)): #vector types
				if len(data) > 8: dt_str = self.get_value() #large vectors use code
				else: dt_str = ', '.join(map(to_str, data)) #small vectors use eval
			else: dt_str = to_str(data)	#other types
			#truncate
			max_len = max(27 - len(self.get_name()), 3)
			if len(dt_str) > max_len:
				dt_str = dt_str[:max_len/2 -3] + '...' + dt_str[-max_len/2:]
			return '<b>%s:</b> %s'%(Utils.xml_encode(self.get_name()), Utils.xml_encode(dt_str))
		else: return '<span foreground="red"><b>%s:</b> error</span>'%Utils.xml_encode(self.get_name())

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
