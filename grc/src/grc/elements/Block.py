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
##@package grc.elements.Block
#Flow graph block.

from grc import Utils
from grc.Utils import odict
from grc.elements.Element import Element
from grc.elements.Param import Param
from grc.elements.Port import Port

from Cheetah.Template import Template
from UserDict import UserDict

class TemplateArg(UserDict):
	"""
	A cheetah template argument created from a param.
	The str of this class evaluates to the param's to code method.
	The use of this class as a dictionary (enum only) will reveal the enum opts.
	The eval method can return the param evaluated to a raw python data type.
	"""

	def __init__(self, param):
		UserDict.__init__(self)
		self._param = param
		if param.is_enum():
			for key in param.get_opt_keys():
				self[key] = str(param.get_opt(key))

	def __str__(self):
		return str(self._param.to_code())

	def eval(self):
		return self._param.evaluate()

class Block(Element):

	def __init__(self, flow_graph, n):
		"""
		Make a new block from nested data.
		@param flow graph the parent element
		@param n the nested odict
		@return block a new block
		"""
		#grab the data
		name = n['name']
		key = n['key']
		category = Utils.exists_or_else(n, 'category', '')
		params = Utils.listify(n, 'param')
		sources = Utils.listify(n, 'source')
		sinks = Utils.listify(n, 'sink')
		#build the block
		Element.__init__(self, flow_graph)
		#store the data
		self._name = name
		self._key = key
		self._category = category
		#create the param objects
		self._params = odict()
		#add the id param
		self._params['id'] = self.get_parent().get_parent().Param(
			self,
			{
				'name': 'ID',
				'key': 'id',
				'type': 'id',
			}
		)
		self._params['_enabled'] = self.get_parent().get_parent().Param(
			self,
			{
				'name': 'Enabled',
				'key': '_enabled',
				'type': 'raw',
				'value': 'True',
				'hide': 'all',
			}
		)
		for param in map(lambda n: self.get_parent().get_parent().Param(self, n), params):
			key = param.get_key()
			#test against repeated keys
			try: assert(key not in self.get_param_keys())
			except AssertionError: self._exit_with_error('Key "%s" already exists in params'%key)
			#store the param
			self._params[key] = param
		#create the source objects
		self._sources = odict()
		for source in map(lambda n: self.get_parent().get_parent().Source(self, n), sources):
			key = source.get_key()
			#test against repeated keys
			try: assert(key not in self.get_source_keys())
			except AssertionError: self._exit_with_error('Key "%s" already exists in sources'%key)
			#store the port
			self._sources[key] = source
		#create the sink objects
		self._sinks = odict()
		for sink in map(lambda n: self.get_parent().get_parent().Sink(self, n), sinks):
			key = sink.get_key()
			#test against repeated keys
			try: assert(key not in self.get_sink_keys())
			except AssertionError: self._exit_with_error('Key "%s" already exists in sinks'%key)
			#store the port
			self._sinks[key] = sink
		#begin the testing
		self.test()

	def test(self):
		"""!
		Call test on all children.
		"""
		map(lambda c: c.test(), self.get_params() + self.get_sinks() + self.get_sources())

	def get_enabled(self):
		"""!
		Get the enabled state of the block.
		@return true for enabled
		"""
		try: return eval(self.get_param('_enabled').get_value())
		except: return True

	def set_enabled(self, enabled):
		"""!
		Set the enabled state of the block.
		@param enabled true for enabled
		"""
		self.get_param('_enabled').set_value(str(enabled))

	def validate(self):
		"""
		Validate the block.
		All ports and params must be valid.
		All checks must evaluate to true.
		"""
		if not self.get_enabled(): return
		for c in self.get_params() + self.get_sinks() + self.get_sources():
			try: assert(c.is_valid())
			except AssertionError:
				for msg in c.get_error_messages():
					self._add_error_message('%s: %s'%(c, msg))

	def __str__(self): return 'Block - %s - %s(%s)'%(self.get_id(), self.get_name(), self.get_key())

	def get_id(self): return self.get_param('id').get_value()

	def is_block(self): return True

	def get_doc(self): return self._doc

	def get_name(self): return self._name

	def get_key(self): return self._key

	def get_category(self): return self._category

	def get_doc(self): return ''

	def get_ports(self): return self.get_sources() + self.get_sinks()

	##############################################
	# Access Params
	##############################################
	def get_param_keys(self): return self._params.keys()
	def get_param(self, key): return self._params[key]
	def get_params(self): return self._params.values()

	##############################################
	# Access Sinks
	##############################################
	def get_sink_keys(self): return self._sinks.keys()
	def get_sink(self, key): return self._sinks[key]
	def get_sinks(self): return self._sinks.values()

	##############################################
	# Access Sources
	##############################################
	def get_source_keys(self): return self._sources.keys()
	def get_source(self, key): return self._sources[key]
	def get_sources(self): return self._sources.values()

	def get_connections(self):
		return sum([port.get_connections() for port in self.get_ports()], [])

	def resolve_dependencies(self, tmpl):
		"""
		Resolve a paramater dependency with cheetah templates.
		@param tmpl the string with dependencies
		@return the resolved value
		"""
		tmpl = str(tmpl)
		if '$' not in tmpl: return tmpl
		n = dict((p.get_key(), TemplateArg(p)) for p in self.get_params())
		try: return str(Template(tmpl, n))
		except Exception, e: return "-------->\n%s: %s\n<--------"%(e, tmpl)

	##############################################
	## Import/Export Methods
	##############################################
	def export_data(self):
		"""
		Export this block's params to nested data.
		@return a nested data odict
		"""
		n = odict()
		n['key'] = self.get_key()
		n['param'] = map(lambda p: p.export_data(), self.get_params())
		return n

	def import_data(self, n):
		"""
		Import this block's params from nested data.
		Any param keys that do not exist will be ignored.
		@param n the nested data odict
		"""
		params_n = Utils.listify(n, 'param')
		for param_n in params_n:
			#key and value must exist in the n data
			if 'key' in param_n.keys() and 'value' in param_n.keys():
				key = param_n['key']
				value = param_n['value']
				#the key must exist in this block's params
				if key in self.get_param_keys():
					self.get_param(key).set_value(value)
		self.validate()
