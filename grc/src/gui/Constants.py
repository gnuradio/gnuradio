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

import pygtk
pygtk.require('2.0')
import gtk
import os

##default path for the open/save dialogs
DEFAULT_FILE_PATH = os.getcwd()

##file extensions
IMAGE_FILE_EXTENSION = '.png'

##name for new/unsaved flow graphs
NEW_FLOGRAPH_TITLE = 'untitled'

##main window constraints
MIN_WINDOW_WIDTH = 600
MIN_WINDOW_HEIGHT = 400
##dialog constraints
MIN_DIALOG_WIDTH = 500
MIN_DIALOG_HEIGHT = 500
##default sizes
DEFAULT_BLOCKS_WINDOW_WIDTH = 100
DEFAULT_REPORTS_WINDOW_WIDTH = 100

##The size of the state saving cache in the flow graph (for undo/redo functionality)
STATE_CACHE_SIZE = 42

##Shared targets for drag and drop of blocks
DND_TARGETS = [('STRING', gtk.TARGET_SAME_APP, 0)]
