# Copyright 2008, 2009, 2015, 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

from . import Platform, FlowGraph, Block

def lazy_property(func):
    return func


class Element(object):

    def __init__(self, parent=None):
        ...

    @property
    def parent(self):
        ...

    def get_parent_by_type(self, cls):
        parent = self.parent
        if parent is None:
            return None
        elif isinstance(parent, cls):
            return parent
        else:
            return parent.get_parent_by_type(cls)

    @lazy_property
    def parent_platform(self): -> Platform.Platform
        ...

    @lazy_property
    def parent_flowgraph(self): -> FlowGraph.FlowGraph
        ...

    @lazy_property
    def parent_block(self): -> Block.Block
        ...


