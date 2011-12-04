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

from .. base.Block import Block as _Block
from .. gui.Block import Block as _GUIBlock
import extract_docs

class Block(_Block, _GUIBlock):

	def is_virtual_sink(self): return self.get_key() == 'virtual_sink'
	def is_virtual_source(self): return self.get_key() == 'virtual_source'

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
		self._doc = n.find('doc') or ''
		self._imports = map(lambda i: i.strip(), n.findall('import'))
		self._make = n.find('make')
		self._var_make = n.find('var_make')
		self._checks = n.findall('check')
		self._callbacks = n.findall('callback')
		self._throttle = n.find('throttle') or ''
		#build the block
		_Block.__init__(
			self,
			flow_graph=flow_graph,
			n=n,
		)
		_GUIBlock.__init__(self)

	def throttle(self): return bool(self._throttle)

	def validate(self):
		"""
		Validate this block.
		Call the base class validate.
		Evaluate the checks: each check must evaluate to True.
		"""
		_Block.validate(self)
		#evaluate the checks
		for check in self._checks:
			check_res = self.resolve_dependencies(check)
			try:
				if not self.get_parent().evaluate(check_res):
					self.add_error_message('Check "%s" failed.'%check)
			except: self.add_error_message('Check "%s" did not evaluate.'%check)

	def rewrite(self):
		"""
		Add and remove ports to adjust for the nports.
		"""
		_Block.rewrite(self)

		def rectify(ports):
			#restore integer contiguity after insertion
			#rectify the port names with the index
			for i, port in enumerate(ports):
				port._key = str(i)
				port._name = port._n['name']
				if len(ports) > 1: port._name += str(i)

		def insert_port(get_ports, get_port, key):
			prev_port = get_port(str(int(key)-1))
			get_ports().insert(
				get_ports().index(prev_port)+1,
				prev_port.copy(new_key=key),
			)
			rectify(get_ports())

		def remove_port(get_ports, get_port, key):
			port = get_port(key)
			for connection in port.get_connections():
				self.get_parent().remove_element(connection)
			get_ports().remove(port)
			rectify(get_ports())

		#adjust nports
		for get_ports, get_port in (
			(self.get_sources, self.get_source),
			(self.get_sinks, self.get_sink),
		):
			master_ports = filter(lambda p: p.get_nports(), get_ports())
			for i, master_port in enumerate(master_ports):
				nports = master_port.get_nports()
				index_first = get_ports().index(master_port)
				try: index_last = get_ports().index(master_ports[i+1])
				except IndexError: index_last = len(get_ports())
				num_ports = index_last - index_first
				#do nothing if nports is already num ports
				if nports == num_ports: continue
				#remove excess ports and connections
				if nports < num_ports:
					for key in reversed(map(str, range(index_first+nports, index_first+num_ports))):
						remove_port(get_ports, get_port, key)
					continue
				#add more ports
				if nports > num_ports:
					for key in map(str, range(index_first+num_ports, index_first+nports)):
						insert_port(get_ports, get_port, key)
					continue

	def port_controller_modify(self, direction):
		"""
		Change the port controller.
		@param direction +1 or -1
		@return true for change
		"""
		changed = False
		#concat the nports string from the private nports settings of all ports
		nports_str = ' '.join([port._nports for port in self.get_ports()])
		#modify all params whose keys appear in the nports string
		for param in self.get_params():
			if param.is_enum() or param.get_key() not in nports_str: continue
			#try to increment the port controller by direction
			try:
				value = param.get_evaluated()
				value = value + direction
				if 0 < value:
					param.set_value(value)
					changed = True
			except: pass
		return changed

	def get_doc(self):
		doc = self._doc.strip('\n').replace('\\\n', '')
		#merge custom doc with doxygen docs
		return '\n'.join([doc, extract_docs.extract(self.get_key())]).strip('\n')

	def get_category(self):
		return _Block.get_category(self)

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
	def get_var_make(self): return self.resolve_dependencies(self._var_make)

	def get_callbacks(self):
		"""
		Get a list of function callbacks for this block.
		@return a list of strings
		"""
		def make_callback(callback):
			callback = self.resolve_dependencies(callback)
			if 'self.' in callback: return callback
			return 'self.%s.%s'%(self.get_id(), callback)
		return map(make_callback, self._callbacks)
