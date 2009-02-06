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

from .. base.Block import Block as _Block
from utils import extract_docs
from ... import utils

class Block(_Block):

	##for make source to keep track of indexes
	_source_count = 0
	##for make sink to keep track of indexes
	_sink_count = 0

	def __init__(self, flow_graph, n):
		"""
		Make a new block from nested data.
		@param flow graph the parent element
		@param n the nested odict
		@return block a new block
		"""
		#grab the data
		doc = utils.exists_or_else(n, 'doc', '')
		imports = map(lambda i: i.strip(), utils.listify(n, 'import'))
		make = n['make']
		checks = utils.listify(n, 'check')
		callbacks = utils.listify(n, 'callback')
		#build the block
		_Block.__init__(
			self,
			flow_graph=flow_graph,
			n=n,
		)
		self._doc = doc
		self._imports = imports
		self._make = make
		self._callbacks = callbacks
		self._checks = checks

	def validate(self):
		"""
		Validate this block.
		Call the base class validate.
		Evaluate the checks: each check must evaluate to True.
		Adjust the nports.
		"""
		_Block.validate(self)
		#evaluate the checks
		for check in self._checks:
			check_res = self.resolve_dependencies(check)
			try:
				check_eval = self.get_parent().evaluate(check_res)
				try: assert check_eval
				except AssertionError: self._add_error_message('Check "%s" failed.'%check)
			except: self._add_error_message('Check "%s" did not evaluate.'%check)
		#adjust nports
		for ports, Port in (
			(self._sources, self.get_parent().get_parent().Source),
			(self._sinks, self.get_parent().get_parent().Sink),
		):
			#how many ports?
			num_ports = len(ports)
			#do nothing for 0 ports
			if not num_ports: continue
			#get the nports setting
			port0 = ports[str(0)]
			nports = port0.get_nports()
			#do nothing for no nports
			if not nports: continue
			#do nothing if nports is already num ports
			if nports == num_ports: continue
			#remove excess ports and connections
			if nports < num_ports:
				#remove the connections
				for key in map(str, range(nports, num_ports)):
					port = ports[key]
					for connection in port.get_connections():
						self.get_parent().remove_element(connection)
				#remove the ports
				for key in map(str, range(nports, num_ports)): ports.pop(key)
				continue
			#add more ports
			if nports > num_ports:
				for key in map(str, range(num_ports, nports)):
					n = port0._n
					n['key'] = key
					port = Port(self, n)
					ports[key] = port
				continue

	def get_doc(self):
		doc = self._doc.strip('\n').replace('\\\n', '')
		#merge custom doc with doxygen docs
		return '\n'.join([doc, extract_docs.extract(self.get_key())]).strip('\n')

	def get_imports(self):
		"""
		Resolve all import statements.
		Split each import statement at newlines.
		Combine all import statments into a list.
		Filter empty imports.
		@return a list of import statements
		"""
		return filter(lambda i: i, sum(map(lambda i: self.resolve_dependencies(i).split('\n'), self._imports), []))

	def get_make(self): return self.resolve_dependencies(self._make)

	def get_callbacks(self):
		"""
		Get a list of function callbacks for this block.
		@return a list of strings
		"""
		return map(lambda c: self.get_id() + '.' + self.resolve_dependencies(c), self._callbacks)
