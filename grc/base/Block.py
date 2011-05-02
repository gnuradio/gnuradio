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

from Cheetah.Template import Template
from UserDict import UserDict

class TemplateArg(UserDict):
	"""
	A cheetah template argument created from a param.
	The str of this class evaluates to the param's to code method.
	The use of this class as a dictionary (enum only) will reveal the enum opts.
	The __call__ or () method can return the param evaluated to a raw python data type.
	"""

	def __init__(self, param):
		UserDict.__init__(self)
		self._param = param
		if param.is_enum():
			for key in param.get_opt_keys():
				self[key] = str(param.get_opt(key))

	def __str__(self):
		return str(self._param.to_code())

	def __call__(self):
		return self._param.get_evaluated()

def _get_keys(lst): return [elem.get_key() for elem in lst]
def _get_elem(lst, key):
	try: return lst[_get_keys(lst).index(key)]
	except ValueError: raise ValueError, 'Key "%s" not found in %s.'%(key, _get_keys(lst))

class Block(Element):

	def __init__(self, flow_graph, n):
		"""
		Make a new block from nested data.
		@param flow graph the parent element
		@param n the nested odict
		@return block a new block
		"""
		#build the block
		Element.__init__(self, flow_graph)
		#grab the data
		params = n.findall('param')
		sources = n.findall('source')
		sinks = n.findall('sink')
		self._name = n.find('name')
		self._key = n.find('key')
		self._category = n.find('category') or ''
		self._block_wrapper_path = n.find('block_wrapper_path')
		#create the param objects
		self._params = list()
		#add the id param
		self.get_params().append(self.get_parent().get_parent().Param(
			block=self,
			n=odict({
				'name': 'ID',
				'key': 'id',
				'type': 'id',
			})
		))
		self.get_params().append(self.get_parent().get_parent().Param(
			block=self,
			n=odict({
				'name': 'Enabled',
				'key': '_enabled',
				'type': 'raw',
				'value': 'True',
				'hide': 'all',
			})
		))
		for param in map(lambda n: self.get_parent().get_parent().Param(block=self, n=n), params):
			key = param.get_key()
			#test against repeated keys
			if key in self.get_param_keys():
				raise Exception, 'Key "%s" already exists in params'%key
			#store the param
			self.get_params().append(param)
		#create the source objects
		self._sources = list()
		for source in map(lambda n: self.get_parent().get_parent().Port(block=self, n=n, dir='source'), sources):
			key = source.get_key()
			#test against repeated keys
			if key in self.get_source_keys():
				raise Exception, 'Key "%s" already exists in sources'%key
			#store the port
			self.get_sources().append(source)
		#create the sink objects
		self._sinks = list()
		for sink in map(lambda n: self.get_parent().get_parent().Port(block=self, n=n, dir='sink'), sinks):
			key = sink.get_key()
			#test against repeated keys
			if key in self.get_sink_keys():
				raise Exception, 'Key "%s" already exists in sinks'%key
			#store the port
			self.get_sinks().append(sink)

	def get_enabled(self):
		"""
		Get the enabled state of the block.
		@return true for enabled
		"""
		try: return eval(self.get_param('_enabled').get_value())
		except: return True

	def set_enabled(self, enabled):
		"""
		Set the enabled state of the block.
		@param enabled true for enabled
		"""
		self.get_param('_enabled').set_value(str(enabled))

	def __str__(self): return 'Block - %s - %s(%s)'%(self.get_id(), self.get_name(), self.get_key())

	def get_id(self): return self.get_param('id').get_value()
	def is_block(self): return True
	def get_name(self): return self._name
	def get_key(self): return self._key
	def get_category(self): return self._category
	def get_doc(self): return ''
	def get_ports(self): return self.get_sources() + self.get_sinks()
	def get_children(self): return self.get_ports() + self.get_params()
	def get_block_wrapper_path(self): return self._block_wrapper_path

	##############################################
	# Access Params
	##############################################
	def get_param_keys(self): return _get_keys(self._params)
	def get_param(self, key): return _get_elem(self._params, key)
	def get_params(self): return self._params

	##############################################
	# Access Sinks
	##############################################
	def get_sink_keys(self): return _get_keys(self._sinks)
	def get_sink(self, key): return _get_elem(self._sinks, key)
	def get_sinks(self): return self._sinks

	##############################################
	# Access Sources
	##############################################
	def get_source_keys(self): return _get_keys(self._sources)
	def get_source(self, key): return _get_elem(self._sources, key)
	def get_sources(self): return self._sources

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
	# Controller Modify
	##############################################
	def type_controller_modify(self, direction):
		"""
		Change the type controller.
		@param direction +1 or -1
		@return true for change
		"""
		changed = False
		type_param = None
		for param in filter(lambda p: p.is_enum(), self.get_params()):
			children = self.get_ports() + self.get_params()
			#priority to the type controller
			if param.get_key() in ' '.join(map(lambda p: p._type, children)): type_param = param
			#use param if type param is unset
			if not type_param: type_param = param
		if type_param:
			#try to increment the enum by direction
			try:
				keys = type_param.get_option_keys()
				old_index = keys.index(type_param.get_value())
				new_index = (old_index + direction + len(keys))%len(keys)
				type_param.set_value(keys[new_index])
				changed = True
			except: pass
		return changed

	def port_controller_modify(self, direction):
		"""
		Change the port controller.
		@param direction +1 or -1
		@return true for change
		"""
		return False

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
		Since params can be dynamically created based another param,
		call rewrite, and repeat the load until the params stick.
		This call to rewrite will also create any dynamic ports
		that are needed for the connections creation phase.
		@param n the nested data odict
		"""
		get_hash = lambda: hash(tuple(map(hash, self.get_params())))
		my_hash = 0
		while get_hash() != my_hash:
			params_n = n.findall('param')
			for param_n in params_n:
				key = param_n.find('key')
				value = param_n.find('value')
				#the key must exist in this block's params
				if key in self.get_param_keys():
					self.get_param(key).set_value(value)
			#store hash and call rewrite
			my_hash = get_hash()
			self.rewrite()
