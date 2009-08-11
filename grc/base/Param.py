"""
Copyright 2008, 2009 Free Software Foundation, Inc.
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

from . import odict
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

class Option(Element):

	def __init__(self, param, n):
		Element.__init__(self, param)
		self._name = n.find('name')
		self._key = n.find('key')
		self._opts = dict()
		opts = n.findall('opt')
		#test against opts when non enum
		try: assert self.get_parent().is_enum() or not opts
		except AssertionError: raise Exception, 'Options for non-enum types cannot have sub-options'
		#extract opts
		for opt in opts:
			#separate the key:value
			try: key, value = opt.split(':')
			except: raise Exception, 'Error separating "%s" into key:value'%opt
			#test against repeated keys
			try: assert not self._opts.has_key(key)
			except AssertionError: raise Exception, 'Key "%s" already exists in option'%key
			#store the option
			self._opts[key] = value

	def __str__(self): return 'Option %s(%s)'%(self.get_name(), self.get_key())
	def get_name(self): return self._name
	def get_key(self): return self._key

	##############################################
	# Access Opts
	##############################################
	def get_opt_keys(self): return self._opts.keys()
	def get_opt(self, key): return self._opts[key]
	def get_opts(self): return self._opts.values()

class Param(Element):

	##possible param types
	TYPES = ['enum', 'raw']

	def __init__(self, block, n):
		"""
		Make a new param from nested data.
		@param block the parent element
		@param n the nested odict
		@return a new param
		"""
		#grab the data
		self._name = n.find('name')
		self._key = n.find('key')
		value = n.find('value') or ''
		self._type = n.find('type')
		self._hide = n.find('hide') or ''
		#build the param
		Element.__init__(self, block)
		#create the Option objects from the n data
		self._options = odict()
		for option in map(lambda o: Option(self, o), n.findall('option')):
			key = option.get_key()
			#test against repeated keys
			try: assert key not in self.get_option_keys()
			except AssertionError: raise Exception, 'Key "%s" already exists in options'%key
			#store the option
			self._options[key] = option
		#test the enum options
		if self.is_enum():
			#test against options with identical keys
			try: assert len(set(self.get_option_keys())) == len(self._options)
			except AssertionError: raise Exception, 'Options keys "%s" are not unique.'%self.get_option_keys()
			#test against inconsistent keys in options
			opt_keys = self._options.values()[0].get_opt_keys()
			for option in self._options.values():
				try: assert set(opt_keys) == set(option.get_opt_keys())
				except AssertionError: raise Exception, 'Opt keys "%s" are not identical across all options.'%opt_keys
			#if a value is specified, it must be in the options keys
			self._value = value or self.get_option_keys()[0]
			try: assert self.get_value() in self.get_option_keys()
			except AssertionError: raise Exception, 'The value "%s" is not in the possible values of "%s".'%(self.get_value(), self.get_option_keys())
		else: self._value = value or ''
		#begin the testing
		self.test()

	def test(self):
		"""
		call test on all children
		"""
		map(lambda c: c.test(), self.get_options())

	def validate(self):
		"""
		Validate the param.
		The value must be evaluated and type must a possible type.
		"""
		Element.validate(self)
		try: assert self.get_type() in self.TYPES
		except AssertionError: self.add_error_message('Type "%s" is not a possible type.'%self.get_type())

	def get_evaluated(self): raise NotImplementedError

	def to_code(self):
		"""
		Convert the value to code.
		@throw NotImplementedError
		"""
		raise NotImplementedError

	def get_color(self): return '#FFFFFF'
	def __str__(self): return 'Param - %s(%s)'%(self.get_name(), self.get_key())
	def is_param(self): return True
	def get_name(self): return self._name
	def get_key(self): return self._key
	def get_hide(self): return self.get_parent().resolve_dependencies(self._hide)

	def get_value(self):
		value = self._value
		if self.is_enum() and value not in self.get_option_keys():
			value = self.get_option_keys()[0]
			self.set_value(value)
		return value

	def set_value(self, value):
		self.flag()
		self._value = str(value) #must be a string

	def get_type(self): return self.get_parent().resolve_dependencies(self._type)
	def is_enum(self): return self._type == 'enum'

	def __repr__(self):
		"""
		Get the repr (nice string format) for this param.
		Just return the value (special case enum).
		Derived classes can handle complex formatting.
		@return the string representation
		"""
		if self.is_enum(): return self.get_option(self.get_value()).get_name()
		return self.get_value()

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

	##############################################
	# Access Options
	##############################################
	def get_option_keys(self): return self._options.keys()
	def get_option(self, key): return self._options[key]
	def get_options(self): return self._options.values()

	##############################################
	# Access Opts
	##############################################
	def get_opt_keys(self): return self._options[self.get_value()].get_opt_keys()
	def get_opt(self, key): return self._options[self.get_value()].get_opt(key)
	def get_opts(self): return self._options[self.get_value()].get_opts()

	##############################################
	## Import/Export Methods
	##############################################
	def export_data(self):
		"""
		Export this param's key/value.
		@return a nested data odict
		"""
		n = odict()
		n['key'] = self.get_key()
		n['value'] = self.get_value()
		return n
