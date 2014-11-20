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

from . Element import Element
from . FlowGraph import FlowGraph as _FlowGraph
from . Connection import Connection as _Connection
from . Block import Block as _Block
from . Port import Port as _Port
from . Param import Param as _Param

from .. model.Platform import Platform as PlatformModel
from .. model.Constants import PREFS_FILE


class Platform(Element, PlatformModel):
    def __init__(self, prefs_file=PREFS_FILE):
        Element.__init__(self)
        self._prefs_file = prefs_file
        PlatformModel.__init__(self)

    def get_prefs_file(self):
        return self._prefs_file


    ##############################################
    # Constructors
    ##############################################
    FlowGraph = _FlowGraph
    Connection = _Connection
    Block = _Block
    Port = _Port
    Param = _Param
