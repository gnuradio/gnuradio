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

from Element import Element
from ... utils import odict

class Connection(Element):

	def __init__(self, flow_graph, porta, portb):
		"""
		Make a new connection given the parent and 2 ports.
		@param flow_graph the parent of this element
		@param porta a port (any direction)
		@param portb a port (any direction)
		@throws Error cannot make connection
		@return a new connection
		"""
		Element.__init__(self, flow_graph)
		source = sink = None
		#separate the source and sink
		for port in (porta, portb):
			if port.is_source(): source = port
			if port.is_sink(): sink = port
		#verify the source and sink
		assert(source and sink)
		assert(not source.is_full())
		assert(not sink.is_full())
		self._source = source
		self._sink = sink

	def __str__(self): return 'Connection (%s -> %s)'%(self.get_source(), self.get_sink())

	def is_connection(self): return True

	def validate(self):
		"""
		Validate the connections.
		The ports must match in type.
		"""
		source_type = self.get_source().get_type()
		sink_type = self.get_sink().get_type()
		try: assert source_type == sink_type
		except AssertionError: self._add_error_message('Source type "%s" does not match sink type "%s".'%(source_type, sink_type))

	def get_enabled(self):
		"""
		Get the enabled state of this connection.
		@return true if source and sink blocks are enabled
		"""
		return self.get_source().get_parent().get_enabled() and \
			self.get_sink().get_parent().get_enabled()

	#############################
	# Access Ports
	#############################
	def get_sink(self): return self._sink
	def get_source(self): return self._source

	##############################################
	## Import/Export Methods
	##############################################
	def export_data(self):
		"""
		Export this connection's info.
		@return a nested data odict
		"""
		n = odict()
		n['source_block_id'] = self.get_source().get_parent().get_id()
		n['sink_block_id'] = self.get_sink().get_parent().get_id()
		n['source_key'] = self.get_source().get_key()
		n['sink_key'] = self.get_sink().get_key()
		return n
