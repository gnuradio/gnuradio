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

from .Config import Config as _Config
from .Block import Block as _Block
from .Connection import Connection as _Connection
from .Element import Element
from .FlowGraph import FlowGraph as _FlowGraph
from .Param import Param as _Param
from .Port import Port as _Port


class Platform(Element, _Platform):

    def __init__(self, *args, **kwargs):
        Element.__init__(self)
        _Platform.__init__(self, *args, **kwargs)

        # Ensure conf directories
        gui_prefs_file = self.config.gui_prefs_file
        if not os.path.exists(os.path.dirname(gui_prefs_file)):
            os.mkdir(os.path.dirname(gui_prefs_file))

        self._move_old_pref_file()

    def get_prefs_file(self):
        return self.config.gui_prefs_file

    def _move_old_pref_file(self):
        gui_prefs_file = self.config.gui_prefs_file
        old_gui_prefs_file = os.environ.get(
            'GRC_PREFS_PATH', os.path.expanduser('~/.grc'))
        if gui_prefs_file == old_gui_prefs_file:
            return  # prefs file overridden with env var
        if os.path.exists(old_gui_prefs_file) and not os.path.exists(gui_prefs_file):
            try:
                import shutil
                shutil.move(old_gui_prefs_file, gui_prefs_file)
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
    Config = _Config
