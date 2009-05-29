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

from utils import expr_utils
from .. base.FlowGraph import FlowGraph as _FlowGraph
from Block import Block
from Connection import Connection

def _get_value_expr(variable_block):
	"""
	Get the expression to evaluate from the value param.
	Parameter blocks need to be evaluated so the stringify flag can be determined.
	@param variable_block the variable or parameter block
	@return the expression string
	"""
	value_param = variable_block.get_param('value')
	if variable_block.get_key() == 'parameter': value_param.evaluate()
	return value_param.to_code()

class FlowGraph(_FlowGraph):

	_eval_cache = dict()
	def _eval(self, code, namespace):
		"""
		Evaluate the code with the given namespace.
		@param code a string with python code
		@param namespace a dict representing the namespace
		@return the resultant object
		"""
		#check cache
		if self._eval_cache.has_key(code) and self._eval_cache[code][0] == namespace:
			return self._eval_cache[code][1]
		#evaluate
		result = eval(code, namespace, namespace)
		self._eval_cache[code] = (namespace.copy(), result)
		return result

	def _get_io_signature(self, pad_key):
		"""
		Get an io signature for this flow graph.
		The pad key determines the directionality of the io signature.
		@param pad_key a string of pad_source or pad_sink
		@return a dict with: type, nports, vlen, size
		"""
		pads = filter(lambda b: b.get_key() == pad_key, self.get_enabled_blocks())
		if not pads: return {
			'nports': '0',
			'type': '',
			'vlen': '0',
			'size': '0',
		}
		pad = pads[0] #take only the first, user should not have more than 1
		#load io signature
		return {
			'nports': str(pad.get_param('nports').evaluate()),
			'type': str(pad.get_param('type').evaluate()),
			'vlen': str(pad.get_param('vlen').evaluate()),
			'size': pad.get_param('type').get_opt('size'),
		}

	def get_input_signature(self):
		"""
		Get the io signature for the input side of this flow graph.
		The io signature with be "0", "0" if no pad source is present.
		@return a string tuple of type, num_ports, port_size
		"""
		return self._get_io_signature('pad_source')

	def get_output_signature(self):
		"""
		Get the io signature for the output side of this flow graph.
		The io signature with be "0", "0" if no pad sink is present.
		@return a string tuple of type, num_ports, port_size
		"""
		return self._get_io_signature('pad_sink')

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
		variables = filter(lambda b: b.get_key() in (
			'variable', 'variable_slider', 'variable_chooser', 'variable_text_box'
		), self.get_enabled_blocks())
		#map var id to variable block
		id2var = dict([(var.get_id(), var) for var in variables])
		#map var id to variable code
		#variable code is a concatenation of all param code (without the id param)
		id2expr = dict([(var.get_id(), var.get_param('value').get_value()) for var in variables])
		#sort according to dependency
		sorted_ids = expr_utils.sort_variables(id2expr)
		#create list of sorted variable blocks
		variables = [id2var[id] for id in sorted_ids]
		return variables

	def get_parameters(self):
		"""
		Get a list of all paramterized variables in this flow graph namespace.
		@return a list of paramterized variables
		"""
		parameters = filter(lambda b: b.get_key() == 'parameter', self.get_enabled_blocks())
		return parameters

	def evaluate(self, expr):
		"""
		Evaluate the expression.
		@param expr the string expression
		@throw Exception bad expression
		@return the evaluated data
		"""
		if self.is_flagged():
			self.deflag()
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
					e = self._eval(_get_value_expr(parameter), n)
					np[parameter.get_id()] = e
				except: pass
			n.update(np) #merge param namespace
			#load variables
			for variable in self.get_variables():
				try:
					e = self._eval(_get_value_expr(variable), n)
					n[variable.get_id()] = e
				except: pass
			#make namespace public
			self.n = n
		#evaluate
		e = self._eval(expr, self.n)
		return e
