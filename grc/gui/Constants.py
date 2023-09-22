"""
Copyright 2008, 2009 Free Software Foundation, Inc.
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

"""


from gi.repository import Gtk, Gdk

from ..core.Constants import *


# default path for the open/save dialogs
DEFAULT_FILE_PATH = os.getcwd() if os.name != 'nt' else os.path.expanduser("~/Documents")
FILE_EXTENSION = '.grc'

# name for new/unsaved flow graphs
NEW_FLOGRAPH_TITLE = 'untitled'

# main window constraints
MIN_WINDOW_WIDTH = 600
MIN_WINDOW_HEIGHT = 400
# dialog constraints
MIN_DIALOG_WIDTH = 600
MIN_DIALOG_HEIGHT = 500
# default sizes
DEFAULT_BLOCKS_WINDOW_WIDTH = 100
DEFAULT_CONSOLE_WINDOW_WIDTH = 100

FONT_SIZE = DEFAULT_FONT_SIZE = 8
FONT_FAMILY = "Sans"
BLOCK_FONT = PORT_FONT = "Sans 8"
PARAM_FONT = "Sans 7.5"

# size of the state saving cache in the flow graph (undo/redo functionality)
STATE_CACHE_SIZE = 42

# Shared targets for drag and drop of blocks
DND_TARGETS = [Gtk.TargetEntry.new('STRING', Gtk.TargetFlags.SAME_APP, 0),
               Gtk.TargetEntry.new('UTF8_STRING', Gtk.TargetFlags.SAME_APP, 1)]

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
PORT_EXTRA_BUS_HEIGHT = 40

# minimal length of connector
CONNECTOR_EXTENSION_MINIMAL = 11

# increment length for connector
CONNECTOR_EXTENSION_INCREMENT = 11

# connection arrow dimensions
CONNECTOR_ARROW_BASE = 10
CONNECTOR_ARROW_HEIGHT = 13

# possible rotations in degrees
POSSIBLE_ROTATIONS = (0, 90, 180, 270)

# How close the mouse can get to the edge of the visible window before scrolling is invoked.
SCROLL_PROXIMITY_SENSITIVITY = 50

# When the window has to be scrolled, move it this distance in the required direction.
SCROLL_DISTANCE = 15

# How close the mouse click can be to a line and register a connection select.
LINE_SELECT_SENSITIVITY = 5

DEFAULT_BLOCK_MODULE_TOOLTIP = """\
This subtree holds all blocks (from OOT modules) that specify no module name. \
The module name is the root category enclosed in square brackets.

Please consider contacting OOT module maintainer for any block in here \
and kindly ask to update their GRC Block Descriptions or Block Tree to include a module name."""


# _SCREEN = Gdk.Screen.get_default()
# _SCREEN_RESOLUTION = _SCREEN.get_resolution() if _SCREEN else -1
# DPI_SCALING = _SCREEN_RESOLUTION / 96.0 if _SCREEN_RESOLUTION > 0 else 1.0
# todo: figure out the GTK3 way (maybe cairo does this for us
DPI_SCALING = 1.0

# Gtk-themes classified as dark
GTK_DARK_THEMES = [
    'Adwaita-dark',
    'HighContrastInverse',
]

GTK_SETTINGS_INI_PATH = '~/.config/gtk-3.0/settings.ini'

GTK_INI_PREFER_DARK_KEY = 'gtk-application-prefer-dark-theme'
GTK_INI_THEME_NAME_KEY = 'gtk-theme-name'


def update_font_size(font_size):
    global PORT_SEPARATION, BLOCK_FONT, PORT_FONT, PARAM_FONT, FONT_SIZE

    FONT_SIZE = font_size
    BLOCK_FONT = "%s %f" % (FONT_FAMILY, font_size)
    PORT_FONT = BLOCK_FONT
    PARAM_FONT = "%s %f" % (FONT_FAMILY, font_size - 0.5)

    PORT_SEPARATION = PORT_SPACING + 2 * \
        PORT_LABEL_PADDING + int(1.5 * font_size)
    PORT_SEPARATION += - \
        PORT_SEPARATION % (2 * CANVAS_GRID_SIZE)  # even multiple


update_font_size(DEFAULT_FONT_SIZE)
