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

from .. base.Port import Port as _Port
from ... import utils

class Port(_Port):

	##possible port types
	TYPES = ['complex', 'float', 'int', 'short', 'byte']

	def __init__(self, block, n):
		"""
		Make a new port from nested data.
		@param block the parent element
		@param n the nested odict
		@return a new port
		"""
		vlen = utils.exists_or_else(n, 'vlen', '1')
		nports = utils.exists_or_else(n, 'nports', '')
		optional = utils.exists_or_else(n, 'optional', '')
		#build the port
		_Port.__init__(
			self,
			block=block,
			n=n,
		)
		self._nports = nports
		self._vlen = vlen
		self._optional = bool(optional)

	def get_vlen(self):
		"""
		Get the vector length.
		If the evaluation of vlen cannot be cast to an integer, return 1.
		@return the vector length or 1
		"""
		vlen = self.get_parent().resolve_dependencies(self._vlen)
		try: return int(self.get_parent().get_parent().evaluate(vlen))
		except: return 1

	def get_nports(self):
		"""
		Get the number of ports.
		If already blank, return a blank
		If the evaluation of nports cannot be cast to an integer, return 1.
		@return the number of ports or 1
		"""
		nports = self.get_parent().resolve_dependencies(self._nports)
		#return blank if nports is blank
		if not nports: return ''
		try:
			nports = int(self.get_parent().get_parent().evaluate(nports))
			assert 0 < nports
			return nports
		except: return 1

	def get_optional(self): return bool(self._optional)

	def get_color(self):
		"""
		Get the color that represents this port's type.
		Codes differ for ports where the vec length is 1 or greater than 1.
		@return a hex color code.
		"""
		try:
			if self.get_vlen() == 1:
				return {#vlen is 1
					'complex': '#3399FF',
					'float': '#FF8C69',
					'int': '#00FF99',
					'short': '#FFFF66',
					'byte': '#FF66FF',
				}[self.get_type()]
			return {#vlen is non 1
				'complex': '#3399AA',
				'float': '#CC8C69',
				'int': '#00CC99',
				'short': '#CCCC33',
				'byte': '#CC66CC',
			}[self.get_type()]
		except: return _Port.get_color(self)

	def is_empty(self):
		"""
		Is this port empty?
		An empty port has no connections.
		Not empty of optional is set.
		@return true if empty
		"""
		return not self.get_optional() and not self.get_connections()

class Source(Port):

	def __init__(self, block, n):
		self._n = n #save n
		#key is port index
		n['key'] = str(block._source_count)
		block._source_count = block._source_count + 1
		Port.__init__(self, block, n)

	def __del__(self):
		self.get_parent()._source_count = self.get_parent()._source_count - 1

class Sink(Port):

	def __init__(self, block, n):
		self._n = n #save n
		#key is port index
		n['key'] = str(block._sink_count)
		block._sink_count = block._sink_count + 1
		Port.__init__(self, block, n)

	def __del__(self):
		self.get_parent()._sink_count = self.get_parent()._sink_count - 1
