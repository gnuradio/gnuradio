#
# Copyright 2003,2004 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

# misc utilities

from __future__ import absolute_import
from __future__ import division
from __future__ import unicode_literals

import types


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
            raise ValueError

    def set_index_by_value(self, v):
        """
        Set index to the smallest value such that items[index] >= v.
        If there is no such item, set index to the maximum value.
        """
        self.set_index(0)              # side effect!
        cv = self.current()
        more = True
        while cv < v and more:
            cv, more = next(self)      # side effect!

    def __next__ (self):
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
