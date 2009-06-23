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

from Element import Element

class Port(Element):

	##possible port types
	TYPES = []

	def __init__(self, block, n):
		"""
		Make a new port from nested data.
		@param block the parent element
		@param n the nested odict
		@return a new port
		"""
		#grab the data
		name = n['name']
		key = n['key']
		type = n['type']
		#build the port
		Element.__init__(self, block)
		self._name = name
		self._key = key
		self._type = type

	def validate(self):
		"""
		Validate the port.
		The port must be non-empty and type must a possible type.
		"""
		Element.validate(self)
		try: assert self.get_type() in self.TYPES
		except AssertionError: self.add_error_message('Type "%s" is not a possible type.'%self.get_type())

	def __str__(self):
		if self.is_source():
			return 'Source - %s(%s)'%(self.get_name(), self.get_key())
		if self.is_sink():
			return 'Sink - %s(%s)'%(self.get_name(), self.get_key())

	def is_port(self): return True
	def get_color(self): return '#FFFFFF'
	def get_name(self): return self._name
	def get_key(self): return self._key
	def is_sink(self): return self in self.get_parent().get_sinks()
	def is_source(self): return self in self.get_parent().get_sources()
	def get_type(self): return self.get_parent().resolve_dependencies(self._type)

	def get_connections(self):
		"""
		Get all connections that use this port.
		@return a list of connection objects
		"""
		connections = self.get_parent().get_parent().get_connections()
		connections = filter(lambda c: c.get_source() is self or c.get_sink() is self, connections)
		return connections

	def get_enabled_connections(self):
		"""
		Get all enabled connections that use this port.
		@return a list of connection objects
		"""
		return filter(lambda c: c.get_enabled(), self.get_connections())
