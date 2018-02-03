"""
Copyright 2008-2015 Free Software Foundation, Inc.
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

    def insert_after(self, pos_key, key, val):
        """
        Insert the new key, value entry after the entry given by the position key.
        If the positional key is None, insert at the end.

        Args:
            pos_key: the positional key
            key: the key for the new entry
            val: the value for the new entry
        """
        index = (pos_key is None) and len(self._keys) or self._keys.index(pos_key)
        if key in self._keys:
            raise KeyError('Cannot insert, key "{0}" already exists'.format(str(key)))
        self._keys.insert(index+1, key)
        self._data[key] = val

    def insert_before(self, pos_key, key, val):
        """
        Insert the new key, value entry before the entry given by the position key.
        If the positional key is None, insert at the beginning.

        Args:
            pos_key: the positional key
            key: the key for the new entry
            val: the value for the new entry
        """
        index = (pos_key is not None) and self._keys.index(pos_key) or 0
        if key in self._keys:
            raise KeyError('Cannot insert, key "{0}" already exists'.format(str(key)))
        self._keys.insert(index, key)
        self._data[key] = val

    def find(self, key):
        """
        Get the value for this key if exists.

        Args:
            key: the key to search for

        Returns:
            the value or None
        """
        if key in self:
            return self[key]
        return None

    def findall(self, key):
        """
        Get a list of values for this key.

        Args:
            key: the key to search for

        Returns:
            a list of values or empty list
        """
        obj = self.find(key)
        if obj is None:
            obj = list()
        if isinstance(obj, list):
            return obj
        return [obj]

    def clear(self):
        self._data.clear()
        del self._keys[:]