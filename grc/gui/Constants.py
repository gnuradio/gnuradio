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

import gtk

from ..core.Constants import *


# default path for the open/save dialogs
DEFAULT_FILE_PATH = os.getcwd() if os.name != 'nt' else os.path.expanduser("~/Documents")

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
DEFAULT_CONSOLE_WINDOW_WIDTH = 100

DEFAULT_CANVAS_SIZE_DEFAULT = 1280, 1024

FONT_SIZE = DEFAULT_FONT_SIZE = 8
FONT_FAMILY = "Sans"
BLOCK_FONT = PORT_FONT = "Sans 8"
PARAM_FONT = "Sans 7.5"

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
PORT_SEPARATION = 32

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

_SCREEN_RESOLUTION = gtk.gdk.screen_get_default().get_resolution()
DPI_SCALING = _SCREEN_RESOLUTION / 96.0 if _SCREEN_RESOLUTION > 0 else 1.0


def update_font_size(font_size):
    global PORT_SEPARATION, BLOCK_FONT, PORT_FONT, PARAM_FONT, FONT_SIZE

    FONT_SIZE = font_size
    BLOCK_FONT = "%s %f" % (FONT_FAMILY, font_size)
    PORT_FONT = BLOCK_FONT
    PARAM_FONT = "%s %f" % (FONT_FAMILY, font_size - 0.5)

    PORT_SEPARATION = PORT_SPACING + 2 * PORT_LABEL_PADDING + int(1.5 * font_size)
    PORT_SEPARATION += -PORT_SEPARATION % (2 * CANVAS_GRID_SIZE)  # even multiple

update_font_size(DEFAULT_FONT_SIZE)
