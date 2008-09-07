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
import gobject
from Constants import PARAM_LABEL_FONT, PARAM_FONT
from os import path

######################################################################################################
# gtk objects for handling input
######################################################################################################

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

class FileParam(EntryParam):
	"""Provide an entry box for filename and a button to browse for a file."""

	def __init__(self, *args, **kwargs):
		EntryParam.__init__(self, *args, **kwargs)
		input = gtk.Button('...')
		input.connect('clicked', self._handle_clicked)
		self.pack_start(input, False)

	def _handle_clicked(self, widget=None):
		"""
		If the button was clicked, open a file dialog in open/save format.
		Replace the text in the entry with the new filename from the file dialog.
		"""
		file_path = self.param.is_valid() and self.param.evaluate() or ''
		#bad file paths will be redirected to default
		if not path.exists(path.dirname(file_path)): file_path = DEFAULT_FILE_PATH
		if self.param.get_type() == 'file_open':
			file_dialog = gtk.FileChooserDialog('Open a Data File...', None,
				gtk.FILE_CHOOSER_ACTION_OPEN, ('gtk-cancel',gtk.RESPONSE_CANCEL,'gtk-open',gtk.RESPONSE_OK))
		elif self.param.get_type() == 'file_save':
			file_dialog = gtk.FileChooserDialog('Save a Data File...', None,
				gtk.FILE_CHOOSER_ACTION_SAVE, ('gtk-cancel',gtk.RESPONSE_CANCEL, 'gtk-save',gtk.RESPONSE_OK))
			file_dialog.set_do_overwrite_confirmation(True)
			file_dialog.set_current_name(path.basename(file_path)) #show the current filename
		file_dialog.set_current_folder(path.dirname(file_path)) #current directory
		file_dialog.set_select_multiple(False)
		file_dialog.set_local_only(True)
		if gtk.RESPONSE_OK == file_dialog.run(): #run the dialog
			file_path = file_dialog.get_filename() #get the file path
			self.entry.set_text(file_path)
			self._handle_changed()
		file_dialog.destroy() #destroy the dialog

class EnumParam(InputParam):
	"""Provide an entry box for Enum types with a drop down menu."""

	def __init__(self, *args, **kwargs):
		InputParam.__init__(self, *args, **kwargs)
		input = gtk.ComboBox(gtk.ListStore(gobject.TYPE_STRING))
		cell = gtk.CellRendererText()
		input.pack_start(cell, True)
		input.add_attribute(cell, 'text', 0)
		for option in self.param.get_options(): input.append_text(option.get_name())
		input.set_active(int(self.param.get_option_keys().index(self.param.get_value())))
		input.connect("changed", self._handle_changed)
		self.pack_start(input, False)
		self.get_text = lambda: str(input.get_active())	#the get text parses the selected index to a string

######################################################################################################
# A Flow Graph Parameter
######################################################################################################

class Param(Element):
	"""The graphical parameter."""

	def update(self):
		"""
		Called when an external change occurs.
		Update the graphical input by calling the change handler.
		"""
		if hasattr(self, 'input'): self._handle_changed()

	def get_input_object(self, callback=None):
		"""
		Get the graphical gtk class to represent this parameter.
		Create the input object with this data type and the handle changed method.
		@param callback a function of one argument(this param) to be called from the change handler
		@return gtk input object
		"""
		self.callback = callback
		if self.is_enum(): input = EnumParam
		elif self.get_type() in ('file_open', 'file_save'): input = FileParam
		else: input = EntryParam
		self.input = input(self, self._handle_changed)
		if not callback: self.update()
		return self.input

	def _handle_changed(self, widget=None):
		"""
		When the input changes, write the inputs to the data type.
		Finish by calling the exteral callback.
		"""
		value = self.input.get_text()
		if self.is_enum(): value = self.get_option_keys()[int(value)]
		self.set_value(value)
		#set the markup on the label, red for errors in corresponding data type.
		name = '<span font_desc="%s">%s</span>'%(PARAM_LABEL_FONT, Utils.xml_encode(self.get_name()))
		#special markups if param is involved in a callback
		if hasattr(self.get_parent(), 'get_callbacks') and \
			filter(lambda c: self.get_key() in c, self.get_parent()._callbacks):
			name = '<span underline="low">%s</span>'%name
		if not self.is_valid():
			self.input.set_markup('<span foreground="red">%s</span>'%name)
			tip = '- ' + '\n- '.join(self.get_error_messages())
		else:
			self.input.set_markup(name)
			tip = self.evaluate()
		#hide/show
		if self.get_hide() == 'all': self.input.hide_all()
		else: self.input.show_all()
		#set the tooltip
		if self.input.tp: self.input.tp.set_tip(self.input.entry, str(tip))
		#execute the external callback
		if self.callback: self.callback(self)

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
				if var.imag == var.real == 0: return '0' #value is zero
				elif var.imag == 0: return '%s'%float_to_str(var.real) #value is real
				elif var.real == 0: return '%sj'%float_to_str(var.imag) #value is imaginary
				elif var.imag < 0: return '%s-%sj'%(float_to_str(var.real), float_to_str(var.imag*-1))
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
				dt_str = ', '.join(map(to_str, data))
			else: dt_str = to_str(data)	#other types
			#truncate
			max_len = max(42 - len(self.get_name()), 3)
			if len(dt_str) > max_len:
				dt_str = dt_str[:max_len-3] + '...'
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
