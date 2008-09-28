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

import os

##default path for the open/save dialogs
DEFAULT_FILE_PATH = os.getcwd()

##file extensions
FLOW_GRAPH_FILE_EXTENSION = '.grc'
IMAGE_FILE_EXTENSION = '.png'

##name for new/unsaved flow graphs
NEW_FLOGRAPH_TITLE = 'untitled'

##rotation constants
DIR_LEFT = 'left'
DIR_RIGHT = 'right'

##main window constraints
MIN_WINDOW_WIDTH = 600
MIN_WINDOW_HEIGHT = 400
##dialog constraints
MIN_DIALOG_WIDTH = 500
MIN_DIALOG_HEIGHT = 500
##static height of reports window
REPORTS_WINDOW_HEIGHT = 100
##static width of block selection window
BLOCK_SELECTION_WINDOW_WIDTH = 200

##How close can the mouse get to the window border before mouse events are ignored.
BORDER_PROXIMITY_SENSITIVITY = 50
##How close the mouse can get to the edge of the visible window before scrolling is invoked.
SCROLL_PROXIMITY_SENSITIVITY = 30
##When the window has to be scrolled, move it this distance in the required direction.
SCROLL_DISTANCE = 15
##The redrawing sensitivity, how many seconds must pass between motion events before a redraw?
MOTION_DETECT_REDRAWING_SENSITIVITY = .02
##How close the mouse click can be to a connection and register a connection select.
CONNECTION_SELECT_SENSITIVITY = 5

##The size of the state saving cache in the flow graph (for undo/redo functionality)
STATE_CACHE_SIZE = 42
