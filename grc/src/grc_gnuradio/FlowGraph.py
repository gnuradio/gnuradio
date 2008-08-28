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
##@package grc_gnuradio.FlowGraph
#Primative flow graph.

from utils import expr_utils
from grc.elements.FlowGraph import FlowGraph as _FlowGraph
from Block import Block
from Connection import Connection
import traceback

class FlowGraph(_FlowGraph):

	def _get_io_signature(self, pad_key):
		"""!
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
		"""!
		Get the io signature for the input side of this flow graph.
		The io signature with be "0", "0" if no pad source is present.
		@return a string tuple of type, num_ports, port_size
		"""
		return self._get_io_signature('pad_source')

	def get_output_signature(self):
		"""!
		Get the io signature for the output side of this flow graph.
		The io signature with be "0", "0" if no pad sink is present.
		@return a string tuple of type, num_ports, port_size
		"""
		return self._get_io_signature('pad_sink')

	def get_imports(self):
		"""!
		Get a set of all import statments in this flow graph namespace.
		@return a set of import statements
		"""
		imports = sum([block.get_imports() for block in self.get_enabled_blocks()], [])
		imports = sorted(set(imports))
		return imports

	def get_variables(self):
		"""!
		Get a list of all variables in this flow graph namespace.
		Exclude paramterized variables.
		@return a sorted list of variable blocks in order of dependency (indep -> dep)
		"""
		try:
			variables = filter(lambda b: b.get_key() in (
				'variable', 'variable_slider', 'variable_chooser', 'variable_text_box'
			), self.get_enabled_blocks())
			#map var id to variable block
			id2var = dict([(var.get_id(), var) for var in variables])
			#map var id to variable code
			#variable code is a concatenation of all param code (without the id param)
			id2expr = dict([(var.get_id(), 
				' '.join([param.to_code() for param in filter(lambda p: p.get_key() != 'id',var.get_params())])
			) for var in variables])
			#sort according to dependency
			sorted_ids = expr_utils.sort_variables(id2expr)
			#create list of sorted variable blocks
			variables = [id2var[id] for id in sorted_ids]
			return variables
		except: traceback.print_exc()

	def get_parameters(self):
		"""!
		Get a list of all paramterized variables in this flow graph namespace.
		@return a list of paramterized variables
		"""
		parameters = filter(lambda b: b.get_key() == 'parameter', self.get_enabled_blocks())
		return parameters

	def evaluate(self, expr):
		"""!
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
					e = eval(parameter.get_param('value').to_code(), n, n)
					np[parameter.get_id()] = e
				except: pass
			n.update(np) #merge param namespace
			#load variables
			for variable in self.get_variables():
				try:
					if variable.get_key() == 'variable_chooser':
						choices = variable.get_param('choices').to_code()
						value_index = variable.get_param('value_index').to_code()
						e = eval("%s[%s]"%(choices, value_index), n, n)
					else:
						e = eval(variable.get_param('value').to_code(), n, n)
					n[variable.get_id()] = e
				except: pass
			#make namespace public
			self.n = n
		#evaluate
		e = eval(expr, self.n, self.n)
		return e

