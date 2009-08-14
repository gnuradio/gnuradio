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

from .. base.Port import Port as _Port
import Constants

class Port(_Port):

	##possible port types
	TYPES = ['complex', 'float', 'int', 'short', 'byte', 'msg']

	def __init__(self, block, n):
		"""
		Make a new port from nested data.
		@param block the parent element
		@param n the nested odict
		"""
		#build the port
		_Port.__init__(
			self,
			block=block,
			n=n,
		)
		self._nports = n.find('nports') or ''
		self._vlen = n.find('vlen') or '1'
		self._optional = bool(n.find('optional'))

	def validate(self):
		_Port.validate(self)
		try: assert self.get_enabled_connections() or self.get_optional()
		except AssertionError: self.add_error_message('Port is not connected.')
		try: assert self.is_source() or len(self.get_enabled_connections()) <= 1
		except AssertionError: self.add_error_message('Port has too many connections.')

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
					'complex': Constants.COMPLEX_COLOR_SPEC,
					'float': Constants.FLOAT_COLOR_SPEC,
					'int': Constants.INT_COLOR_SPEC,
					'short': Constants.SHORT_COLOR_SPEC,
					'byte': Constants.BYTE_COLOR_SPEC,
					'msg': Constants.MSG_COLOR_SPEC,
				}[self.get_type()]
			return {#vlen is non 1
				'complex': Constants.COMPLEX_VECTOR_COLOR_SPEC,
				'float': Constants.FLOAT_VECTOR_COLOR_SPEC,
				'int': Constants.INT_VECTOR_COLOR_SPEC,
				'short': Constants.SHORT_VECTOR_COLOR_SPEC,
				'byte': Constants.BYTE_VECTOR_COLOR_SPEC,
			}[self.get_type()]
		except: return _Port.get_color(self)

class Source(Port):

	def __init__(self, block, n):
		self._n = n #save n
		if n['type'] == 'msg': n['key'] = 'msg'
		else:
			n['key'] = str(block._source_count)
			block._source_count = block._source_count + 1
		Port.__init__(self, block, n)

	def __del__(self):
		self.get_parent()._source_count = self.get_parent()._source_count - 1

class Sink(Port):

	def __init__(self, block, n):
		self._n = n #save n
		if n['type'] == 'msg': n['key'] = 'msg'
		else:
			n['key'] = str(block._sink_count)
			block._sink_count = block._sink_count + 1
		Port.__init__(self, block, n)

	def __del__(self):
		self.get_parent()._sink_count = self.get_parent()._sink_count - 1

#TODO check that nports and vlen is undefined when type is message
#TODO only allow up to one port of type msg
