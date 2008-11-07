"""
Copyright 2008 Free Software Foundation, Inc.
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

from ... import utils
from ... utils import odict
from Element import Element
import pygtk
pygtk.require('2.0')
import gtk
import gobject

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
		input = gtk.ComboBox(gtk.ListStore(gobject.TYPE_STRING))
		cell = gtk.CellRendererText()
		input.pack_start(cell, True)
		input.add_attribute(cell, 'text', 0)
		for option in self.param.get_options(): input.append_text(option.get_name())
		input.set_active(int(self.param.get_option_keys().index(self.param.get_value())))
		input.connect("changed", self._handle_changed)
		self.pack_start(input, False)
		self.get_text = lambda: str(input.get_active())	#the get text parses the selected index to a string

class Option(Element):

	def __init__(self, param, name, key, opts):
		Element.__init__(self, param)
		self._name = name
		self._key = key
		self._opts = dict()
		for opt in opts:
			#separate the key:value
			try: key, value = opt.split(':')
			except: self._exit_with_error('Error separating "%s" into key:value'%opt)
			#test against repeated keys
			try: assert(not self._opts.has_key(key))
			except AssertionError: self._exit_with_error('Key "%s" already exists in option'%key)
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

	##############################################
	## Static Make Methods
	##############################################
	def make_option_from_n(param, n):
		"""
		Make a new option from nested data.
		@param param the parent element
		@param n the nested odict
		@return a new option
		"""
		#grab the data
		name = n['name']
		key = n['key']
		opts = utils.listify(n, 'opt')
		#build the option
		return Option(
			param=param,
			name=name,
			key=key,
			opts=opts,
		)
	make_option_from_n = staticmethod(make_option_from_n)

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
		name = n['name']
		key = n['key']
		value = utils.exists_or_else(n, 'value', '')
		type = n['type']
		hide = utils.exists_or_else(n, 'hide', '')
		options = utils.listify(n, 'option')
		#build the param
		Element.__init__(self, block)
		self._name = name
		self._key = key
		self._type = type
		self._hide = hide
		#create the Option objects from the n data
		self._options = odict()
		for option in map(lambda o: Option.make_option_from_n(self, o), options):
			key = option.get_key()
			#test against repeated keys
			try: assert(key not in self.get_option_keys())
			except AssertionError: self._exit_with_error('Key "%s" already exists in options'%key)
			#store the option
			self._options[key] = option
		#test the enum options
		if self._options or self.is_enum():
			#test against bad combos of type and enum
			try: assert(self._options)
			except AssertionError: self._exit_with_error('At least one option must exist when type "enum" is set.')
			try: assert(self.is_enum())
			except AssertionError: self._exit_with_error('Type "enum" must be set when options are present.')
			#test against options with identical keys
			try: assert(len(set(self.get_option_keys())) == len(self._options))
			except AssertionError: self._exit_with_error('Options keys "%s" are not unique.'%self.get_option_keys())
			#test against inconsistent keys in options
			opt_keys = self._options.values()[0].get_opt_keys()
			for option in self._options.values():
				try: assert(set(opt_keys) == set(option.get_opt_keys()))
				except AssertionError: self._exit_with_error('Opt keys "%s" are not identical across all options.'%opt_keys)
			#if a value is specified, it must be in the options keys
			self._value = value or self.get_option_keys()[0]
			try: assert(self.get_value() in self.get_option_keys())
			except AssertionError: self._exit_with_error('The value "%s" is not in the possible values of "%s".'%(self.get_value(), self.get_option_keys()))
		else: self._value = value or ''

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
		try:
			assert(self.get_type() in self.TYPES)
			try: self.evaluate()
			except:
				#if the evaluate failed but added no error messages, add the generic one below
				if not self.get_error_messages():
					self._add_error_message('Value "%s" cannot be evaluated.'%self.get_value())
		except AssertionError: self._add_error_message('Type "%s" is not a possible type.'%self.get_type())

	def evaluate(self):
		"""
		Evaluate the value of this param.
		@throw NotImplementedError
		"""
		raise NotImplementedError

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

	def get_input_class(self):
		"""
		Get the graphical gtk class to represent this parameter.
		@return gtk input class
		"""
		if self.is_enum(): return EnumParam
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
