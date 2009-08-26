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

class Element(object):

	def __init__(self, parent=None):
		self._parent = parent
		self.flag()

	def test(self):
		"""
		Test the element against failures.
		Overload this method in sub-classes.
		"""
		pass

	##################################################
	# Element Validation API
	##################################################
	def validate(self): self._error_messages = list()
	def is_valid(self): return not self.get_error_messages() or not self.get_enabled()
	def add_error_message(self, msg): self._error_messages.append(msg)
	def get_error_messages(self): return self._error_messages

	def rewrite(self): pass

	def get_enabled(self): return True

	def get_parent(self): return self._parent

	##############################################
	## Update flagging
	##############################################
	def is_flagged(self): return self._flag
	def flag(self):
		self._flag = True
		if self.get_parent(): self.get_parent().flag()
	def deflag(self):
		self._flag = False
		if self.get_parent(): self.get_parent().deflag()

	##############################################
	## Type testing methods
	##############################################
	def is_element(self): return True
	def is_platform(self): return False
	def is_flow_graph(self): return False
	def is_connection(self): return False
	def is_block(self): return False
	def is_source(self): return False
	def is_sink(self): return False
	def is_port(self): return False
	def is_param(self): return False
