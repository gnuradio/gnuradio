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
from .. gui.Port import Port as _GUIPort
import Constants

def _get_source_from_virtual_sink_port(vsp):
	"""
	Resolve the source port that is connected to the given virtual sink port.
	Use the get source from virtual source to recursively resolve subsequent ports. 
	"""
	try: return _get_source_from_virtual_source_port(
		vsp.get_enabled_connections()[0].get_source())
	except: raise Exception, 'Could not resolve source for virtual sink port %s'%vsp

def _get_source_from_virtual_source_port(vsp, traversed=[]):
	"""
	Recursively resolve source ports over the virtual connections.
	Keep track of traversed sources to avoid recursive loops.
	"""
	if not vsp.get_parent().is_virtual_source(): return vsp
	if vsp in traversed: raise Exception, 'Loop found when resolving virtual source %s'%vsp
	try: return _get_source_from_virtual_source_port(
		_get_source_from_virtual_sink_port(
			filter(#get all virtual sinks with a matching stream id
				lambda vs: vs.get_param('stream_id').get_value() == vsp.get_parent().get_param('stream_id').get_value(),
				filter(#get all enabled blocks that are also virtual sinks
					lambda b: b.is_virtual_sink(),
					vsp.get_parent().get_parent().get_enabled_blocks(),
				),
			)[0].get_sinks()[0]
		), traversed + [vsp],
	)
	except: raise Exception, 'Could not resolve source for virtual source port %s'%vsp

class Port(_Port, _GUIPort):

	def __init__(self, block, n, dir):
		"""
		Make a new port from nested data.
		@param block the parent element
		@param n the nested odict
		@param dir the direction
		"""
		self._n = n
		if n['type'] == 'msg': n['key'] = 'msg'
		if dir == 'source' and not n.find('key'):
			n['key'] = str(block._source_count)
			block._source_count += 1
		if dir == 'sink' and not n.find('key'):
			n['key'] = str(block._sink_count)
			block._sink_count += 1
		#build the port
		_Port.__init__(
			self,
			block=block,
			n=n,
			dir=dir,
		)
		_GUIPort.__init__(self)
		self._nports = n.find('nports') or ''
		self._vlen = n.find('vlen') or ''
		self._optional = bool(n.find('optional'))

	def get_types(self): return ('complex', 'float', 'int', 'short', 'byte', 'msg', '')

	def validate(self):
		_Port.validate(self)
		try: assert self.get_enabled_connections() or self.get_optional()
		except AssertionError: self.add_error_message('Port is not connected.')
		try: assert self.is_source() or len(self.get_enabled_connections()) <= 1
		except AssertionError: self.add_error_message('Port has too many connections.')
		#message port logic
		if self.get_type() == 'msg':
			try: assert not self.get_nports()
			except AssertionError: self.add_error_message('A port of type "msg" cannot have "nports" set.')
			try: assert self.get_vlen() == 1
			except AssertionError: self.add_error_message('A port of type "msg" must have a "vlen" of 1.')

	def rewrite(self):
		"""
		Handle the port cloning for virtual blocks.
		"""
		_Port.rewrite(self)
		if self.get_parent().is_virtual_sink() or self.get_parent().is_virtual_source():
			try: #clone type and vlen
				source = self.resolve_virtual_source()
				self._type = str(source.get_type())
				self._vlen = str(source.get_vlen())
			except: #reset type and vlen
				self._type = ''
				self._vlen = ''

	def resolve_virtual_source(self):
		if self.get_parent().is_virtual_sink(): return _get_source_from_virtual_sink_port(self)
		if self.get_parent().is_virtual_source(): return _get_source_from_virtual_source_port(self)

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

	def copy(self, new_key=None):
		n = self._n.copy()
		#remove nports from the key so the copy cannot be a duplicator
		if n.has_key('nports'): n.pop('nports')
		if new_key: n['key'] = new_key
		return self.__class__(self.get_parent(), n, self._dir)
