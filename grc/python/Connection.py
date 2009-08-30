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

from .. base.Connection import Connection as _Connection
from .. gui.Connection import Connection as _GUIConnection

class Connection(_Connection, _GUIConnection):

	def __init__(self, **kwargs):
		_Connection.__init__(self, **kwargs)
		_GUIConnection.__init__(self)

	def is_msg(self):
		return self.get_source().get_type() == self.get_sink().get_type() == 'msg'

	def validate(self):
		"""
		Validate the connections.
		The ports must match in type and vector length.
		"""
		_Connection.validate(self) #checks type
		#check vector length
		source_vlen = self.get_source().get_vlen()
		sink_vlen = self.get_sink().get_vlen()
		try: assert source_vlen == sink_vlen
		except AssertionError: self.add_error_message('Source vector length "%s" does not match sink vector length "%s".'%(source_vlen, sink_vlen))
