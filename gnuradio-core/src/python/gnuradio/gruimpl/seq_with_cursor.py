#
# Copyright 2003,2004 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

# misc utilities

import types
import exceptions

class seq_with_cursor (object):
    __slots__ = [ 'items', 'index' ]

    def __init__ (self, items, initial_index = None, initial_value = None):
        assert len (items) > 0, "seq_with_cursor: len (items) == 0"
        self.items = items
        self.set_index (initial_index)
        if initial_value is not None:
            self.set_index_by_value(initial_value)

    def set_index (self, initial_index):
        if initial_index is None:
            self.index = len (self.items) / 2
        elif initial_index >= 0 and initial_index < len (self.items):
            self.index = initial_index
        else:
            raise exceptions.ValueError

    def set_index_by_value(self, v):
        """
        Set index to the smallest value such that items[index] >= v.
        If there is no such item, set index to the maximum value.
        """
        self.set_index(0)              # side effect!
        cv = self.current()
        more = True
        while cv < v and more:
            cv, more = self.next()      # side effect!

    def next (self):
        new_index = self.index + 1
        if new_index < len (self.items):
            self.index = new_index
            return self.items[new_index], True
        else:
            return self.items[self.index], False

    def prev (self):
        new_index = self.index - 1
        if new_index >= 0:
            self.index = new_index
            return self.items[new_index], True
        else:
            return self.items[self.index], False

    def current (self):
        return self.items[self.index]

    def get_seq (self):
        return self.items[:]            # copy of items

