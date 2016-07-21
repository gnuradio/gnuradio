# Copyright 2016 Free Software Foundation, Inc.
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

from typing import Union

from . import Platform, FlowGraph, Block

lazy_property = property  # fixme: descriptors don't seems to be supported


class Element(object):

    def __init__(self, parent: Union[None, 'Element'] = None): ...

    @lazy_property
    def parent(self) -> 'Element': ...

    def get_parent_by_type(self, cls) -> Union[None, 'Element']: ...

    @lazy_property
    def parent_platform(self) -> Platform.Platform: ...

    @lazy_property
    def parent_flowgraph(self) -> FlowGraph.FlowGraph: ...

    @lazy_property
    def parent_block(self) -> Block.Block: ...
