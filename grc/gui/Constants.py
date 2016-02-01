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

import pygtk
pygtk.require('2.0')
import gtk

from gnuradio import gr

prefs = gr.prefs()
GR_PREFIX = gr.prefix()
EDITOR = prefs.get_string('grc', 'editor', '')

# default path for the open/save dialogs
DEFAULT_FILE_PATH = os.getcwd()

# file extensions
IMAGE_FILE_EXTENSION = '.png'
TEXT_FILE_EXTENSION = '.txt'

# name for new/unsaved flow graphs
NEW_FLOGRAPH_TITLE = 'untitled'

# main window constraints
MIN_WINDOW_WIDTH = 600
MIN_WINDOW_HEIGHT = 400
# dialog constraints
MIN_DIALOG_WIDTH = 500
MIN_DIALOG_HEIGHT = 500
# default sizes
DEFAULT_BLOCKS_WINDOW_WIDTH = 100
DEFAULT_REPORTS_WINDOW_WIDTH = 100

try:  # ugly, but matches current code style
    raw = prefs.get_string('grc', 'canvas_default_size', '1280, 1024')
    DEFAULT_CANVAS_SIZE = tuple(int(x.strip('() ')) for x in raw.split(','))
    if len(DEFAULT_CANVAS_SIZE) != 2 or not all(300 < x < 4096 for x in DEFAULT_CANVAS_SIZE):
        raise Exception()
except:
    DEFAULT_CANVAS_SIZE = 1280, 1024
    print >> sys.stderr, "Error: invalid 'canvas_default_size' setting."

#  flow-graph canvas fonts
try:  # ugly, but matches current code style
    FONT_SIZE = prefs.get_long('grc', 'canvas_font_size', 8)
    if FONT_SIZE <= 0:
        raise Exception()
except:
    FONT_SIZE = 8
    print >> sys.stderr, "Error: invalid 'canvas_font_size' setting."
FONT_FAMILY = "Sans"
BLOCK_FONT = "%s %f" % (FONT_FAMILY, FONT_SIZE)
PORT_FONT = BLOCK_FONT
PARAM_FONT = "%s %f" % (FONT_FAMILY, FONT_SIZE - 0.5)


# size of the state saving cache in the flow graph (undo/redo functionality)
STATE_CACHE_SIZE = 42

# Shared targets for drag and drop of blocks
DND_TARGETS = [('STRING', gtk.TARGET_SAME_APP, 0)]

# label constraint dimensions
LABEL_SEPARATION = 3
BLOCK_LABEL_PADDING = 7
PORT_LABEL_PADDING = 2

# canvas grid size
CANVAS_GRID_SIZE = 8

# port constraint dimensions
PORT_BORDER_SEPARATION = 8
PORT_SPACING = 2 * PORT_BORDER_SEPARATION
PORT_SEPARATION = PORT_SPACING + 2 * PORT_LABEL_PADDING + int(1.5 * FONT_SIZE)
PORT_SEPARATION += -PORT_SEPARATION % (2 * CANVAS_GRID_SIZE)  # even multiple

PORT_MIN_WIDTH = 20
PORT_LABEL_HIDDEN_WIDTH = 10

# minimal length of connector
CONNECTOR_EXTENSION_MINIMAL = 11

# increment length for connector
CONNECTOR_EXTENSION_INCREMENT = 11

# connection arrow dimensions
CONNECTOR_ARROW_BASE = 13
CONNECTOR_ARROW_HEIGHT = 17

# possible rotations in degrees
POSSIBLE_ROTATIONS = (0, 90, 180, 270)

# How close can the mouse get to the window border before mouse events are ignored.
BORDER_PROXIMITY_SENSITIVITY = 50

# How close the mouse can get to the edge of the visible window before scrolling is invoked.
SCROLL_PROXIMITY_SENSITIVITY = 30

# When the window has to be scrolled, move it this distance in the required direction.
SCROLL_DISTANCE = 15

# How close the mouse click can be to a line and register a connection select.
LINE_SELECT_SENSITIVITY = 5
