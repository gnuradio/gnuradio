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
##@package grc.gui.Utils
#Utility methods and classes.

from UserDict import DictMixin

class odict(DictMixin):

	def __init__(self, d={}):
		self._keys = list(d.keys())
		self._data = dict(d.copy())

	def __setitem__(self, key, value):
		if key not in self._data:
			self._keys.append(key)
		self._data[key] = value

	def __getitem__(self, key):
		return self._data[key]

	def __delitem__(self, key):
		del self._data[key]
		self._keys.remove(key)

	def keys(self):
		return list(self._keys)

	def copy(self):
		copy_dict = odict()
		copy_dict._data = self._data.copy()
		copy_dict._keys = list(self._keys)
		return copy_dict

def exists_or_else(d, key, alt):
	if d.has_key(key): return d[key]
	else: return alt

def listify(d, key):
	obj = exists_or_else(d, key, [])
	if isinstance(obj, list): return obj
	return [obj]

