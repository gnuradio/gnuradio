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
##@package grc.elements.FlowGraph
#Primative flow graph.
#@author Josh Blum

from grc import Utils
from grc.Utils import odict
from grc.elements.Element import Element
from grc.elements.Block import Block
from grc.elements.Connection import Connection

from grc import Messages

class FlowGraph(Element):

	def __init__(self, platform):
		"""!
		Make a flow graph from the arguments.
		@param platform a platforms with blocks and contrcutors
		@return the flow graph object
		"""
		#hold connections and blocks
		self._elements = list()
		#initialize
		Element.__init__(self, platform)
		#inital blank import
		self.import_data({'flow_graph': {}})

	def __str__(self): return 'FlowGraph - "%s"'%self.get_option('name')

	def get_option(self, key):
		"""!
		Get the option for a given key.
		The option comes from the special options block.
		@param key the param key for the options block
		@return the value held by that param
		"""
		return self._options_block.get_param(key).evaluate()

	def is_flow_graph(self): return True

	##############################################
	## Access Elements
	##############################################
	def get_block(self, id): return filter(lambda b: b.get_id() == id, self.get_blocks())[0]
	def get_blocks(self): return filter(lambda e: e.is_block(), self.get_elements())
	def get_connections(self): return filter(lambda e: e.is_connection(), self.get_elements())
	def get_elements(self):
		"""!
		Get a list of all the elements.
		Always ensure that the options block is in the list.
		@return the element list
		"""
		if self._options_block not in self._elements: self._elements.append(self._options_block)
		#ensure uniqueness of the elements list
		element_set = set()
		element_list = list()
		for element in self._elements:
			if element not in element_set: element_list.append(element)
			element_set.add(element)
		#store cleaned up list
		self._elements = element_list
		return self._elements

	def get_enabled_blocks(self):
		"""!
		Get a list of all blocks that are enabled.
		@return a list of blocks
		"""
		return filter(lambda b: b.get_enabled(), self.get_blocks())

	def get_enabled_connections(self):
		"""!
		Get a list of all connections that are enabled.
		@return a list of connections
		"""
		return filter(lambda c: c.get_enabled(), self.get_connections())

	def get_new_block(self, key):
		"""!
		Get a new block of the specified key.
		Add the block to the list of elements.
		@param key the block key
		@return the new block or None if not found
		"""
		self.flag()
		if key not in self.get_parent().get_block_keys(): return None
		block = self.get_parent().get_new_block(self, key)
		self.get_elements().append(block)
		return block

	def connect(self, porta, portb):
		"""!
		Create a connection between porta and portb.
		@param porta a port
		@param portb another port
		@throw Exception bad connection
		@return the new connection
		"""
		self.flag()
		connection = self.get_parent().Connection(self, porta, portb)
		self.get_elements().append(connection)
		return connection

	def remove_element(self, element):
		"""!
		Remove the element from the list of elements.
		If the element is a port, remove the whole block.
		If the element is a block, remove its connections.
		If the element is a connection, just remove the connection.
		"""
		self.flag()
		if element not in self.get_elements(): return
		#found a port, set to parent signal block
		if element.is_port():
			element = element.get_parent()
		#remove block, remove all involved connections
		if element.is_block():
			for port in element.get_ports():
				map(lambda c: self.remove_element(c), port.get_connections())
		#remove a connection
		elif element.is_connection(): pass
		self.get_elements().remove(element)

	def evaluate(self, expr):
		"""!
		Evaluate the expression.
		@param expr the string expression
		@throw NotImplementedError
		"""
		raise NotImplementedError

	def validate(self):
		"""
		Validate the flow graph.
		All connections and blocks must be valid.
		"""
		for c in self.get_elements():
			try: assert(c.is_valid())
			except AssertionError: self._add_error_message('Element "%s" is not valid.'%c)

	##############################################
	## Import/Export Methods
	##############################################
	def export_data(self):
		"""
		Export this flow graph to nested data.
		Export all block and connection data.
		@return a nested data odict
		"""
		import time
		n = odict()
		n['timestamp'] = time.ctime()
		n['block'] = [block.export_data() for block in self.get_blocks()]
		n['connection'] = [connection.export_data() for connection in self.get_connections()]
		return {'flow_graph': n}

	def import_data(self, n):
		"""
		Import blocks and connections into this flow graph.
		Clear this flowgraph of all previous blocks and connections.
		Any blocks or connections in error will be ignored.
		@param n the nested data odict
		"""
		#remove previous elements
		self._elements = list()
		#the flow graph tag must exists, or use blank data
		if 'flow_graph' in n.keys(): fg_n = n['flow_graph']
		else:
			Messages.send_error_load('Flow graph data not found, loading blank flow graph.')
			fg_n = {}
		blocks_n = Utils.listify(fg_n, 'block')
		connections_n = Utils.listify(fg_n, 'connection')
		#create option block
		self._options_block = self.get_parent().get_new_block(self, 'options')
		self._options_block.get_param('id').set_value('options')
		#build the blocks
		for block_n in blocks_n:
			key = block_n['key']
			if key == 'options': block = self._options_block
			else: block = self.get_new_block(key)
			#only load the block when the block key was valid
			if block: block.import_data(block_n)
			else: Messages.send_error_load('Block key "%s" not found in %s'%(key, self.get_parent()))
		#build the connections
		for connection_n in connections_n:
			#test that the data tags exist
			try:
				assert('source_block_id' in connection_n.keys())
				assert('sink_block_id' in connection_n.keys())
				assert('source_key' in connection_n.keys())
				assert('sink_key' in connection_n.keys())
			except AssertionError: continue
			#try to make the connection
			try:
				#get the block ids
				source_block_id = connection_n['source_block_id']
				sink_block_id = connection_n['sink_block_id']
				#get the port keys
				source_key = connection_n['source_key']
				sink_key = connection_n['sink_key']
				#verify the blocks
				block_ids = map(lambda b: b.get_id(), self.get_blocks())
				assert(source_block_id in block_ids)
				assert(sink_block_id in block_ids)
				#get the blocks
				source_block = self.get_block(source_block_id)
				sink_block = self.get_block(sink_block_id)
				#verify the ports
				assert(source_key in source_block.get_source_keys())
				assert(sink_key in sink_block.get_sink_keys())
				#get the ports
				source = source_block.get_source(source_key)
				sink = sink_block.get_sink(sink_key)
				#build the connection
				self.connect(source, sink)
			except AssertionError: Messages.send_error_load('Connection between %s(%s) and %s(%s) could not be made.'%(source_block_id, source_key, sink_block_id, sink_key))
		self.validate()

