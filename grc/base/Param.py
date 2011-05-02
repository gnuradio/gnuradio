"""
Copyright 2008-2011 Free Software Foundation, Inc.
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

def _get_keys(lst): return [elem.get_key() for elem in lst]
def _get_elem(lst, key):
	try: return lst[_get_keys(lst).index(key)]
	except ValueError: raise ValueError, 'Key "%s" not found in %s.'%(key, _get_keys(lst))

class Option(Element):

	def __init__(self, param, n):
		Element.__init__(self, param)
		self._name = n.find('name')
		self._key = n.find('key')
		self._opts = dict()
		opts = n.findall('opt')
		#test against opts when non enum
		if not self.get_parent().is_enum() and opts:
			raise Exception, 'Options for non-enum types cannot have sub-options'
		#extract opts
		for opt in opts:
			#separate the key:value
			try: key, value = opt.split(':')
			except: raise Exception, 'Error separating "%s" into key:value'%opt
			#test against repeated keys
			if self._opts.has_key(key):
				raise Exception, 'Key "%s" already exists in option'%key
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

	def __init__(self, block, n):
		"""
		Make a new param from nested data.
		@param block the parent element
		@param n the nested odict
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
		self._options = list()
		for option in map(lambda o: Option(param=self, n=o), n.findall('option')):
			key = option.get_key()
			#test against repeated keys
			if key in self.get_option_keys():
				raise Exception, 'Key "%s" already exists in options'%key
			#store the option
			self.get_options().append(option)
		#test the enum options
		if self.is_enum():
			#test against options with identical keys
			if len(set(self.get_option_keys())) != len(self.get_options()):
				raise Exception, 'Options keys "%s" are not unique.'%self.get_option_keys()
			#test against inconsistent keys in options
			opt_keys = self.get_options()[0].get_opt_keys()
			for option in self.get_options():
				if set(opt_keys) != set(option.get_opt_keys()):
					raise Exception, 'Opt keys "%s" are not identical across all options.'%opt_keys
			#if a value is specified, it must be in the options keys
			self._value = value if value or value in self.get_option_keys() else self.get_option_keys()[0]
			if self.get_value() not in self.get_option_keys():
				raise Exception, 'The value "%s" is not in the possible values of "%s".'%(self.get_value(), self.get_option_keys())
		else: self._value = value or ''

	def validate(self):
		"""
		Validate the param.
		The value must be evaluated and type must a possible type.
		"""
		Element.validate(self)
		if self.get_type() not in self.get_types():
			self.add_error_message('Type "%s" is not a possible type.'%self.get_type())

	def get_evaluated(self): raise NotImplementedError

	def to_code(self):
		"""
		Convert the value to code.
		@throw NotImplementedError
		"""
		raise NotImplementedError

	def get_types(self):
		"""
		Get a list of all possible param types.
		@throw NotImplementedError
		"""
		raise NotImplementedError

	def get_color(self): return '#FFFFFF'
	def __str__(self): return 'Param - %s(%s)'%(self.get_name(), self.get_key())
	def is_param(self): return True
	def get_name(self): return self._name
	def get_key(self): return self._key
	def get_hide(self): return self.get_parent().resolve_dependencies(self._hide).strip()

	def get_value(self):
		value = self._value
		if self.is_enum() and value not in self.get_option_keys():
			value = self.get_option_keys()[0]
			self.set_value(value)
		return value

	def set_value(self, value): self._value = str(value) #must be a string

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

	##############################################
	# Access Options
	##############################################
	def get_option_keys(self): return _get_keys(self.get_options())
	def get_option(self, key): return _get_elem(self.get_options(), key)
	def get_options(self): return self._options

	##############################################
	# Access Opts
	##############################################
	def get_opt_keys(self): return self.get_option(self.get_value()).get_opt_keys()
	def get_opt(self, key): return self.get_option(self.get_value()).get_opt(key)
	def get_opts(self): return self.get_option(self.get_value()).get_opts()

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
