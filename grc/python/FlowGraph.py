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

import expr_utils
from .. base.FlowGraph import FlowGraph as _FlowGraph
from .. gui.FlowGraph import FlowGraph as _GUIFlowGraph
import re

_variable_matcher = re.compile('^(variable\w*)$')
_parameter_matcher = re.compile('^(parameter)$')

class FlowGraph(_FlowGraph, _GUIFlowGraph):

	def __init__(self, **kwargs):
		_FlowGraph.__init__(self, **kwargs)
		_GUIFlowGraph.__init__(self)
		self._eval_cache = dict()

	def _eval(self, code, namespace, namespace_hash):
		"""
		Evaluate the code with the given namespace.
		@param code a string with python code
		@param namespace a dict representing the namespace
		@param namespace_hash a unique hash for the namespace
		@return the resultant object
		"""
		if not code: raise Exception, 'Cannot evaluate empty statement.'
		my_hash = hash(code) ^ namespace_hash
		#cache if does not exist
		if not self._eval_cache.has_key(my_hash):
			self._eval_cache[my_hash] = eval(code, namespace, namespace)
		#return from cache
		return self._eval_cache[my_hash]

	def get_io_signaturev(self, direction):
		"""
		Get a list of io signatures for this flow graph.
		@param direction a string of 'in' or 'out'
		@return a list of dicts with: type, label, vlen, size
		"""
		sorted_pads = {
			'in': self.get_pad_sources(),
			'out': self.get_pad_sinks(),
		}[direction]
		#load io signature
		return [{
			'label': str(pad.get_param('label').get_evaluated()),
			'type': str(pad.get_param('type').get_evaluated()),
			'vlen': str(pad.get_param('vlen').get_evaluated()),
			'size': pad.get_param('type').get_opt('size'),
		} for pad in sorted_pads]

	def get_pad_sources(self):
		"""
		Get a list of pad source blocks sorted by id order.
		@return a list of pad source blocks in this flow graph
		"""
		pads = filter(lambda b: b.get_key() == 'pad_source', self.get_enabled_blocks())
		return sorted(pads, lambda x, y: cmp(x.get_id(), y.get_id()))

	def get_pad_sinks(self):
		"""
		Get a list of pad sink blocks sorted by id order.
		@return a list of pad sink blocks in this flow graph
		"""
		pads = filter(lambda b: b.get_key() == 'pad_sink', self.get_enabled_blocks())
		return sorted(pads, lambda x, y: cmp(x.get_id(), y.get_id()))

	def get_imports(self):
		"""
		Get a set of all import statments in this flow graph namespace.
		@return a set of import statements
		"""
		imports = sum([block.get_imports() for block in self.get_enabled_blocks()], [])
		imports = sorted(set(imports))
		return imports

	def get_variables(self):
		"""
		Get a list of all variables in this flow graph namespace.
		Exclude paramterized variables.
		@return a sorted list of variable blocks in order of dependency (indep -> dep)
		"""
		variables = filter(lambda b: _variable_matcher.match(b.get_key()), self.get_enabled_blocks())
		return expr_utils.sort_objects(variables, lambda v: v.get_id(), lambda v: v.get_var_make())

	def get_parameters(self):
		"""
		Get a list of all paramterized variables in this flow graph namespace.
		@return a list of paramterized variables
		"""
		parameters = filter(lambda b: _parameter_matcher.match(b.get_key()), self.get_enabled_blocks())
		return parameters

	def rewrite(self):
		"""
		Flag the namespace to be renewed.
		"""
		self._renew_eval_ns = True
		_FlowGraph.rewrite(self)

	def evaluate(self, expr):
		"""
		Evaluate the expression.
		@param expr the string expression
		@throw Exception bad expression
		@return the evaluated data
		"""
		if self._renew_eval_ns:
			self._renew_eval_ns = False
			#reload namespace
			n = dict()
			#load imports
			for imp in self.get_imports():
				try: exec imp in n
				except: pass
			#load parameters
			np = dict()
			for parameter in self.get_parameters():
				try:
					e = eval(parameter.get_param('value').to_code(), n, n)
					np[parameter.get_id()] = e
				except: pass
			n.update(np) #merge param namespace
			#load variables
			for variable in self.get_variables():
				try:
					e = eval(variable.get_param('value').to_code(), n, n)
					n[variable.get_id()] = e
				except: pass
			#make namespace public
			self.n = n
			self.n_hash = hash(str(n))
		#evaluate
		e = self._eval(expr, self.n, self.n_hash)
		return e
