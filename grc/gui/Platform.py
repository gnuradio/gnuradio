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

import os
import sys

from ..core.Platform import Platform as _Platform

from .Block import Block as _Block
from .Connection import Connection as _Connection
from .Constants import PREFS_FILE, PREFS_FILE_OLD
from .Element import Element
from .FlowGraph import FlowGraph as _FlowGraph
from .Param import Param as _Param
from .Port import Port as _Port


class Platform(Element, _Platform):

    def __init__(self):
        Element.__init__(self)
        _Platform.__init__(self)

        # Ensure conf directories
        if not os.path.exists(os.path.dirname(PREFS_FILE)):
            os.mkdir(os.path.dirname(PREFS_FILE))

        self._move_old_pref_file()
        self._prefs_file = PREFS_FILE

    def get_prefs_file(self):
        return self._prefs_file

    @staticmethod
    def _move_old_pref_file():
        if PREFS_FILE == PREFS_FILE_OLD:
            return  # prefs file overridden with env var
        if os.path.exists(PREFS_FILE_OLD) and not os.path.exists(PREFS_FILE):
            try:
                import shutil
                shutil.move(PREFS_FILE_OLD, PREFS_FILE)
            except Exception as e:
                print >> sys.stderr, e

    ##############################################
    # Constructors
    ##############################################
    FlowGraph = _FlowGraph
    Connection = _Connection
    Block = _Block
    Port = _Port
    Param = _Param
