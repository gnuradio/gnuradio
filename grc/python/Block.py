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

from .. base.Block import Block as _Block
import extract_docs
import extract_category

class Block(_Block):

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
		#build the block
		_Block.__init__(
			self,
			flow_graph=flow_graph,
			n=n,
		)

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
				check_eval = self.get_parent().evaluate(check_res)
				try: assert check_eval
				except AssertionError: self.add_error_message('Check "%s" failed.'%check)
			except: self.add_error_message('Check "%s" did not evaluate.'%check)

	def rewrite(self):
		"""
		Add and remove ports to adjust for the nports.
		"""
		_Block.rewrite(self)
		#adjust nports
		for get_ports, get_port in (
			(self.get_sources, self.get_source),
			(self.get_sinks, self.get_sink),
		):
			#how many streaming (non-message) ports?
			num_ports = len(filter(lambda p: p.get_type() != 'msg', get_ports()))
			#do nothing for 0 ports
			if not num_ports: continue
			#get the nports setting
			port0 = get_port(str(0))
			nports = port0.get_nports()
			#do nothing for no nports
			if not nports: continue
			#do nothing if nports is already num ports
			if nports == num_ports: continue
			#remove excess ports and connections
			if nports < num_ports:
				#remove the connections
				for key in map(str, range(nports, num_ports)):
					port = get_port(key)
					for connection in port.get_connections():
						self.get_parent().remove_element(connection)
				#remove the ports
				for key in map(str, range(nports, num_ports)):
					get_ports().remove(get_port(key))
				continue
			#add more ports
			if nports > num_ports:
				for key in map(str, range(num_ports, nports)):
					prev_port = get_port(str(int(key)-1))
					get_ports().insert(
						get_ports().index(prev_port)+1,
						prev_port.copy(new_key=key),
					)
				continue

	def port_controller_modify(self, direction):
		"""
		Change the port controller.
		@param direction +1 or -1
		@return true for change
		"""
		changed = False
		#concat the nports string from the private nports settings of both port0
		nports_str = \
			(self.get_sinks() and self.get_sinks()[0]._nports or '') + \
			(self.get_sources() and self.get_sources()[0]._nports or '')
		#modify all params whose keys appear in the nports string
		for param in self.get_params():
			if param.is_enum() or param.get_key() not in nports_str: continue
			#try to increment the port controller by direction
			try:
				value = param.get_evaluated()
				value = value + direction
				assert 0 < value
				param.set_value(value)
				changed = True
			except: pass
		return changed

	def get_doc(self):
		doc = self._doc.strip('\n').replace('\\\n', '')
		#merge custom doc with doxygen docs
		return '\n'.join([doc, extract_docs.extract(self.get_key())]).strip('\n')

	def get_category(self):
		category = extract_category.extract(self.get_key())
		#if category: return category
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
